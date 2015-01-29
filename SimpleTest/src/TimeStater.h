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

// ʱ��ͳ��
class TimeStater
{
public:
     TimeStater(int32_t id)
         :_id(id){}

    // ������Ϣͳ�Ʒ�ʽ
     void SetMsgStatWay(int32_t msg, bool single);

    // ��ʱ��ʼ
     void TimeStart(int32_t msg);

    // ��ʱ����
    bool TimeEnd(int32_t msg,  int32_t critical_ms);

    // ͳ�Ʊ���
    void StatTime(int32_t msg, int32_t critical_ms);
private:
    int32_t _id;
    std::map<int32_t, int64_t> _start_time;

private:
    // ��Ϣͳ�Ʒ�ʽ true: �첽ͳ��(����ͳ��) false: ͬ��ͳ��(����ͳ��)
    static std::map<int32_t, bool> _msg_stat_single;
    // ��Ϣ -> ��Ϣ��Ӧ�����ѵ�ʱ��
    static std::map<int32_t, std::vector<int64_t>> _time_costs;
    // ��Ϣ�������� - ��Ϣ��������
    static std::map<int32_t, std::atomic<unsigned int>> _msg_sendrecv;
    static Locker _lock;
};

#endif
