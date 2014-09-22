#ifndef __NGSERVER_MESSAGEQUEUE_H_INCLUDE__
#define __NGSERVER_MESSAGEQUEUE_H_INCLUDE__

#include<vector>
#include"Locker.h"

/*
 *	文件：	MessageQueue.h
 *	功能：定义消息队列，分自定义消息和字节流消息两种，分别由MessageQueue和ByteBuff实现
 *  线程安全：
 *		消息队列均适用双缓冲来减少竞争和锁
 *		消息队列的推送消息接口(Push PushMsg)线程安全 可在多个线程中执行
 *		取出消息(PopAll)需要保证在处理完PopAll弹出的消息后 再次通过PopAll获取消息。
 *		亦即同一时刻应只有一个线程在执行消息队列中的消息
 */
template <typename MessageT>
class MessageQueue
{
public:
	MessageQueue()
	{
		_queue_reader = new std::vector< MessageT* >();
		_queue_writer = new std::vector< MessageT* >();
	}

public:

	inline size_t Size() const
	{
		return _size;
	}

	// 压入一条消息 压入成功则返回当前消息队列中的消息数量  否则返回0
	size_t PushMsg(MessageT* msg)
	{
		if(msg == nullptr) return 0;

		AutoLocker aLock(&_lock);
		_queue_writer->push_back(msg);
		return ++_size;
	}

	// 弹出当前消息队列中所有待处理的消息
	std::vector< MessageT* >* PopAll()
	{
		if(_size > 0)
		{
			AutoLocker aLock locker(&_lock);
			if(_size > 0)
			{
                assert(_queue_reader->size() == 0);
                _size = 0;
                swap(_queue_writer, _queue_reader);
				return _queue_reader;
			}
		}
		return nullptr;
	}

private:
	Locker _lock;
	size_t _size = 0;
	std::vector< MessageT* >* _queue_reader;
	std::vector< MessageT* >* _queue_writer;
};

// 相当于MessageQueue 对char的特例化
class ByteBuff
{
public:
	ByteBuff(size_t capacity) : _capacity(capacity)
	{
		_buff_read = new char[_capacity];
		_buff_write= new char[_capacity];
	}

public:

	inline size_t Size() const
	{
		return _size;
	}

	// 压入字节流数据 压入成功 则返回当前缓冲区长度 否则返回0
	size_t Push(const char* data, size_t len)
	{
		if(data != nullptr && len > 0)
		{
			AutoLocker aLock(&_lock);
			if(_size+len <=  _capacity)
			{
				memcpy(_buff_write+_size, data, len);
				return _size += len;
			}
		}
		return 0;
	}

	// 返回当前缓冲区指针 长度由len返回 若当前缓冲区无消息 返回nullptr
	char* PopAll(size_t& len)
	{
		if(_size > 0)
		{
			AutoLocker aLock(&_lock);
			if(_size > 0)
			{
				swap(_buff_read, _buff_write);
				len = _size;
				_size = 0;
				return _buff_read;
			}
		}
        len = 0;
		return nullptr;
	}

private:
	size_t _capacity;
	size_t _size;
	
	Locker _lock;

	char* _buff_read;
	char* _buff_write;
};







#endif
