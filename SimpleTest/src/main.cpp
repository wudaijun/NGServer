
#include "LogicClient.h"
#include "../../protocol/protocol.h"
#include <iostream>
#include <fstream>
#include "RobotsManager.h"

using namespace NGServer::protocol;
using namespace std;

void Usage()
{
}

void GetParam(string& server_address, short& server_port, int& num_of_robot, int& login_opt)
{
    /*
    cout << "服务器地址: \n";
    cin >> server_address;

    cout << "服务器端口: \n";
    cin >> server_port;

    cout << "机器人数量: \n";
    cin >> num_of_robot;
    assert(num_of_robot > 0 && num_of_robot < 10000);

    cout << "登录选项： 0 注册 1 登录 ： \n";
    cin >> login_opt;
    */
    
    server_address = "127.0.0.1";
    server_port = 23456;
    login_opt = 0;
    num_of_robot = 1;
    
}

int main(int argc, char* argv[])
{
    string server_address;
    short server_port;
    int num_of_robot;
    int login_opt = 0;

    GetParam(server_address, server_port, num_of_robot, login_opt);

    IOService io;
    for (int i = 0; i < num_of_robot; i++)
    {
        std::shared_ptr<LogicClient> robot = std::make_shared<LogicClient>(&io, i);
        //RobotsManager::GetInstance().AddRobot(robot);
        robot->SetServAddress(server_address, server_port);
        robot->ConnectToServer();
        sleep(1);
    }

    io.run();

    return 0;
}
