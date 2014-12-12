#ifndef __NGSERVER_SESSION_H_INCLUDE__
#define __NGSERVER_SESSION_H_INCLUDE__
#include "NetHead.h"
#include "MessageQueue.h"

typedef boost::asio::socket_base::shutdown_type ShutDownType;


/*
*   �ļ��� Session.h
*   ���ܣ� ����Session��
*   Session�ࣺ
*       ��Ҫ�ṩ�����ӿڣ�
*           StartRecv() �����Ự
*           SendAsync() �첽�������� �ڲ�ʹ��˫��������Ч��
*           ProcessData() ����_recv_buff�е����ݽӿ� һ��������
*
*   TODO: ĿǰSessionͨ��Closeִ�б����ر�( _socket.close() )
*         û���������ŵĹر�TCP���ӣ�������ȷ����ر�����ʱ�Ķ�д������
*          ����Ӧ�ö����Ż�
*/

class Session : public std::enable_shared_from_this<Session>
{
public:
    static const int32_t kBufferSize = 65536; // 64 k ����������

public:
    Session(const std::shared_ptr<Socket> socket, int32_t conn_id)
        : _socket(socket), _id(conn_id), _send_buf(4*kBufferSize){}

    virtual ~Session(){}

    // �����Ự
    void StartRecv();

    // ��������
    bool SendAsync(const char* data, size_t len);

    void SetDecoder(std::function<int32_t(const char* data, size_t len)>& decoder)
    {
        _decoder = decoder;
    }

    // �ͻ��˶Ͽ�����
    // notifyServer: �Ƿ�֪ͨҵ���߼���
    void DisConnect( bool notifyServer = true );

private:
    // �첽��ȡ����
    void AsyncReadSome();
    // ��ȡ��ɣ��������ݲ�������ȡ
    void ReadComplete(const boost::system::error_code& err, size_t bytes_transferred);
    // ������ջ�����������
    bool ProcessData();

    // �������� ͬһʱ��ֻ��һ���߳�ִ�иú�����������
    void SendData(const char* data, size_t len);
    // �������
    void SendComplete(const boost::system::error_code& err, size_t bytes_to_transfer, size_t bytes_transferred);

protected:
    uint32_t _id;   // �ỰID

private:
    std::function<int32_t(const char* data, size_t len)> _decoder;

    std::shared_ptr<Socket> _socket;

    // ���ջ�����
    char _recv_buf[kBufferSize];
    size_t _recv_off = 0;
    size_t _recv_total = 0;

    // ���ͻ�����(˫����)
    ByteBuff _send_buf;
    size_t _send_total = 0;

    // ������ ͬһʱ��ֻ����һ���̷߳�������
    Locker _sending_lock;
    // Session �Ƿ��ڻ��
    Locker _runing_lock;
    // Session �Ƿ��ѹر�
    Locker _closed_lock;
};

typedef std::shared_ptr<Session> SessionPtr;

#endif