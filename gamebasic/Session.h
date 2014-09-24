#ifndef __NGSERVER_SESSION_H_INCLUDE__
#define __NGSERVER_SESSION_H_INCLUDE__
#include "NetHead.h"
#include "MessageQueue.h"

/*
*   �ļ��� Session.h
*   ���ܣ� ����Session��
*   Session�ࣺ
*       ��Ҫ�ṩ�����ӿڣ�
*           StartRecv() �����Ự
*           SendAsync() �첽�������� �ڲ�ʹ��˫��������Ч��
*           ProcessData() ����_recv_buff�е����ݽӿ� һ��������
*/

class Session : public std::enable_shared_from_this<Session>
{
    static const int32_t kBufferSize = 65536; // 64 k ����������

public:
    Session(const std::shared_ptr<Socket> socket, int32_t conn_id)
        : _socket(socket), _id(conn_id), _send_buf(4*kBufferSize){}

    // �����Ự
    void StartRecv();

    // ��������
    bool SendAsync(const char* data, size_t len);

    void SetDecoder(std::function<int32_t(const char* data, size_t len)>& decoder)
    {
        _decoder = decoder;
    }

    void DisConnect();
private:
    // �첽��ȡ����
    void AsyncReadSome();
    // ��ȡ��ɣ��������ݲ�������ȡ
    void ReadComplete(const boost::system::error_code& err, size_t bytes_transferred);
    // ������ջ�����������
    void ProcessData();

    // �������� ͬһʱ��ֻ��һ���߳�ִ�иú�����������
    void SendData(const char* data, size_t len);
    // �������
    void SendComplete(const boost::system::error_code& err, size_t bytes_to_transfer, size_t bytes_transferred);

private:
    std::function<int32_t(const char* data, size_t len)> _decoder;

    std::shared_ptr<Socket> _socket;
    uint32_t _id;   // �ỰID

    bool _run = false; // ״̬��ʶ

    // ���ջ�����
    char _recv_buf[kBufferSize];
    size_t _recv_off = 0;
    size_t _recv_total = 0;

    // ���ͻ�����(˫����)
    ByteBuff _send_buf;
    size_t _send_total = 0;

    // ������ ͬһʱ��ֻ����һ���̷߳�������
    Locker _sending_lock;
    Locker _closing_lock;
};

typedef std::shared_ptr<Session> SessionPtr;

#endif