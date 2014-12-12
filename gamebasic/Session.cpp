#include "Session.h"

void Session::StartRecv()
{
    if (_runing_lock.TryLock())
        AsyncReadSome();
}

void Session::AsyncReadSome()
{
    if (_socket)
        _socket->async_read_some(
            boost::asio::buffer(_recv_buf + _recv_off, kBufferSize - _recv_off),
            boost::bind(&Session::ReadComplete, shared_from_this(),
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
}

void Session::ReadComplete(const boost::system::error_code& err, size_t bytes_transferred)
{
    if (err || bytes_transferred == 0)
    {
        DisConnect();
        return;
    }
    _recv_total += bytes_transferred;
    _recv_off += bytes_transferred;

    // ��������
    if (ProcessData())
    {
        // ������������
        AsyncReadSome();
    }
}

// �Ի������е����ݽ�� ����false��Ͽ�����
bool Session::ProcessData()
{
    if (_decoder == nullptr)
        return false;

    // �����ݽ��ɽ��������� ���ش���֮��Ļ�����ʣ���ֽ��� ����-1��ʾ��������������
    int32_t remain = _decoder(_recv_buf,_recv_off);

    // �������Ͽ�����
    if (remain < 0)
    {
        DisConnect();
        return false;
    }

    // ����֮���ƫ��
    if (remain > 0 && remain < kBufferSize)
    {
        size_t remain_off = _recv_off - remain;
        _recv_off = (size_t)remain;
        memcpy(_recv_buf, _recv_buf + remain_off, _recv_off);
    }
    else
    {
        _recv_off = 0;
    }
    return true;
}

// ��������
bool Session::SendAsync(const char* data, size_t len)
{
    if (_socket == nullptr)
        return false;

    if (_send_buf.Push(data, len))
    {
        if (_sending_lock.TryLock())
        {
            size_t sendlen;
            const char* data = _send_buf.PopAll(sendlen);
            SendData(data, sendlen);
            return true;
        }
    }
    else
    {
        assert(0); // ���ͻ�������
        return false;
    }
}

// �������� ͬһʱ��ֻ��һ���̵߳��øú���
void Session::SendData(const char* data, size_t len)
{
    if (_socket)
        _socket->async_write_some(boost::asio::buffer(data, len),
            boost::bind(&Session::SendComplete, shared_from_this(), 
                                boost::asio::placeholders::error, 
                                len,
                                boost::asio::placeholders::bytes_transferred));
}

void Session::SendComplete(const boost::system::error_code& err, size_t bytes_to_transfer, size_t bytes_transferred)
{
    if (err)
    {
        cout << err.message() << endl;
        return;
    }
    assert(bytes_to_transfer == bytes_transferred);

    _send_total += bytes_transferred;

    size_t len;
    const char* data = _send_buf.PopAll(len);
    if (data) // �����������  ��������
    {
        SendAsync(data, len);
    }
    else
    {
        _sending_lock.UnLock();
    }
}

// �����Ͽ�����
void Session::DisConnect(bool notifyServer)
{
    if (_closed_lock.TryLock())
    {
        if (_decoder)
        {
            // ֪ͨҵ��� �����ѶϿ� ��������߼�ҵ����
            if (notifyServer)
            {
                _decoder(nullptr, 0);
            }
            _decoder = nullptr;
        }

        if (_socket && _socket->available())
            _socket->close();
        _socket = nullptr;
    }
}