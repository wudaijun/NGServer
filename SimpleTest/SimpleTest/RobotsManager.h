#ifndef __SIMPLE_TEST_ROBOTS_MANAGER_H_INCLUDE__
#define __SIMPLE_TEST_ROBOTS_MANAGER_H_INCLUDE__
#include "LogicClient.h"
#include <boost/shared_ptr.hpp>
using namespace std;

class RobotsManager
{
public:
    void AddRobot(const std::shared_ptr<LogicClient>& robot)
    {
        _robots.push_back(robot);
    }

    static RobotsManager& GetInstance()
    {
        static RobotsManager manager;
        return manager;
    }

private:
    RobotsManager(){};
    RobotsManager(const RobotsManager& r) = delete;
    RobotsManager& operator = (const RobotsManager& r) = delete;

private:
    std::vector<std::shared_ptr<LogicClient>> _robots;
};

#endif