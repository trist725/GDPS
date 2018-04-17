#pragma once

#include "ITcpSession.h"

class CGdpSession : public ITcpSession, public boost::enable_shared_from_this<CGdpSession>
{
protected:
	const static size_t MAX_PACKET_SIZE = 1024 * 1024;
public:
	typedef boost::function<bool(shared_ptr<CGdpSession> pSession, byte *pData, uint32 length)> OnReceivePacketCallback;
	typedef boost::function<void(shared_ptr<CGdpSession> pSession)> OnCloseCallback;
public:
	CGdpSession(boost::asio::io_service &ios);
	void Init(const string &id, OnReceivePacketCallback onReceivePacket, OnCloseCallback onClose);
	virtual const string &GetId();
	virtual boost::asio::ip::tcp::socket &GetSocket();
	virtual const boost::asio::ip::tcp::endpoint &GetRemote();
	virtual bool Start();
	virtual void Close();
	virtual void Send(const byte *pData, size_t length);
protected:
	void AsyncSend(shared_ptr<buffer_t> pBuffer);
	void OnSend(const boost::system::error_code &error);
	void ReceiveHead();
	void OnReceiveHead(const boost::system::error_code &error, size_t length);
	void OnReceiveBody(const boost::system::error_code &error, size_t length);
	void AsyncClose();
protected:
	string _id;
	boost::asio::io_service &_ios;
	boost::asio::ip::tcp::socket _socket;
	boost::asio::ip::tcp::endpoint _remote;
	bool _connected;
	buffer_t _sendingBuffer;
	buffer_t _pendingBuffer;
	byte _recvBuffer[MAX_PACKET_SIZE];
	OnReceivePacketCallback _onReceivePacket;
	OnCloseCallback _onClose;
};
