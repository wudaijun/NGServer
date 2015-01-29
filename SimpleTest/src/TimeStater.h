#ifndef __SIMPLE_TEST_TIMESTATER_H_INCLUDE__
#define __SIMPLE_TEST_TIMESTATER_H_INCLUDE__

#include <map>
#include <vector>
#include <chrono>
#include <assert.h>
#include <atomic>
#include <algorithm>
#include <numeric>
#include <iostream>
#include "../../common/Locker.h"

// 时间统计
class TimeStater
{
public:
     TimeStater(int32_t id)
         :_id(id){}

    // 设置消息统计方式
     void SetMsgStatWay(int32_t msg, bool single);

    // 计时开始
     void TimeStart(int32_t msg);

    // 计时结束
    bool TimeEnd(int32_t msg,  int32_t critical_ms);

    // 统计报告
    void StatTime(int32_t msg, int32_t critical_ms);
private:
    int32_t _id;
    std::map<int32_t, int64_t> _start_time;

private:
    // 消息统计方式 true: 异步统计(单独统计) false: 同步统计(批量统计)
    static std::map<int32_t, bool> _msg_stat_single;
    // 消息 -> 消息响应所花费的时间
    static std::map<int32_t, std::vector<int64_t>> _time_costs;
    // 消息发送数量 - 消息接收数量
    static std::map<int32_t, std::atomic<unsigned int>> _msg_sendrecv;
    static Locker _lock;
};

#endif
