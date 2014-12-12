#ifndef __NGSERVER_DBPROTOCOL_H_INCLUDE__
#define __NGSERVER_DBPROTOCOL_H_INCLUDE__

#include "protocol.h"

/*
*   DB���Э�� �Լ������ڲ�ͨ��Э��
*   ����GameService��DBService����Ϣ���̼߳���Ϣ����˶��ڷ�POD��������������
*   ��������ֱ�Ӷ���struct �ں���ϢID ����
*/

namespace NGServer{
namespace protocol{

/****************************/
/*       DBЭ����ϢID       */
/***************************/
enum DBMsgId
{
    kS2D_Start = 10000,
    kS2D_PlayerLogin,           // ��ҵ�¼
    kD2S_PlayerLoginResult,     // ��¼���
    kS2D_LoadPlayer,            // �����������
    kD2S_PlayerLoadCompleted,   // ������ݼ������

};

// ����ϢID ��
#define DBMSGID(id) static const DBMsgId msgid = (NGServer::protocol::DBMsgId::id);

/****************************/
/*  �����ڲ�ͨ��Э����ϢID   */
/***************************/
enum SSMsgId
{
    kSS_Start = 20000,
    kSS_PlayerLogin,    // ��ҵ�¼
};

// ����ϢID ��
#define SSMSGID(id) static const SSMsgId msgid = (NGServer::protocol::SSMsgId::id);

/**************************/
/*        Э������        */
/*************************/
struct S2D_PlayerLogin
{
    DBMSGID(kS2D_PlayerLogin);
    std::string owner;
    std::string pwd;
    uint32_t connid;
};


struct D2S_PlayerLoginResult
{
    DBMSGID(kD2S_PlayerLoginResult);
    std::string owner;
    std::string pwd;
    uint32_t connid;
    int64_t playerid; // ���Ψһ��ʶ
    // һЩ��������
};

struct S2D_LoadPlayer
{
    DBMSGID(kS2D_LoadPlayer);
    int64_t playerid;   // ���ص����ID
    // ....
};

struct D2S_PlayerLoadCompleted
{
    DBMSGID(kD2S_PlayerLoadCompleted);
    int64_t playerid;   // ��ɼ��ص����ID
    // ....
};

}
}
#endif