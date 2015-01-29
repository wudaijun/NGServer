#include "TimeStater.h"
#include "RobotsManager.h"


 std::map<int32_t, std::vector<int64_t>> TimeStater::_time_costs;
 std::map<int32_t, std::atomic<unsigned int>> TimeStater::_msg_sendrecv;
 std::map<int32_t, bool> TimeStater::_msg_stat_single;
 Locker TimeStater::_lock;

 void TimeStater::SetMsgStatWay(int32_t msg, bool single)
 {
     _msg_stat_single[msg] = single;
 }

 // 计时开始
 void TimeStater::TimeStart(int32_t msg)
 {
     // 毫秒级
     _start_time[msg] = std::chrono::steady_clock::now().time_since_epoch().count() / 10000;
     if (!_msg_stat_single[msg])
     {
         _msg_sendrecv[msg] ++;
     }
 }

 // 计时结束 返回值为 true 表示所有请求均已响应完成
 bool TimeStater::TimeEnd(int32_t msg, int32_t critical_ms)
 {
     int64_t now = std::chrono::steady_clock::now().time_since_epoch().count() / 10000;
     int64_t time_cost = now - _start_time[msg];

     if (_msg_stat_single[msg])
     {
         _start_time[msg] = time_cost;
         StatTime(msg, critical_ms);
         return true;
     }
     else
     {
         _lock.Lock();
         _time_costs[msg].push_back(time_cost);
         _msg_sendrecv[msg] --;
         _lock.UnLock();

         //if (_msg_sendrecv[msg] == GetInstance<RobotsManager>().GetActivityCount())
         if (_msg_sendrecv[msg] == 0)
         {
             StatTime(msg, critical_ms);
             return true;
         }
         return false;
     }
 }

 // 统计报告
 void TimeStater::StatTime(int32_t msg, int32_t critical_ms)
 {
     if (_msg_stat_single[msg])
     {
         if (_start_time[msg] > critical_ms)
         {
             std::cout << "### ID: " << msg << " TIME: " << _start_time[msg] << "ms " << std::endl;
         }
     }
     else
     {
         _lock.Lock();
         int64_t min_cost = *std::min_element(_time_costs[msg].begin(), _time_costs[msg].end());
         int64_t max_cost = *std::max_element(_time_costs[msg].begin(), _time_costs[msg].end());
         int64_t ave_cost = std::accumulate(_time_costs[msg].begin(), _time_costs[msg].end(), 0) / _time_costs[msg].size();

         _time_costs[msg].clear();
         _msg_sendrecv[msg] = 0;

         _lock.UnLock();

         std::cout << "--------------- 统计报告 -------------\n";
         std::cout << "  ID : " << msg << "  MIN: " << min_cost << "ms MAX: " << max_cost << "ms AVE: " << ave_cost << "ms\n";
         std::cout << "-------------------------------------\n";
     }
 }
