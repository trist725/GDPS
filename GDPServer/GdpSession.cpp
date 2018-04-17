#include "stdafx.h"
#include "GdpSession.h"

CGdpSession::CGdpSession(boost::asio::io_service &ios) : 
	_ios(ios), _socket(_ios), _connected(false) 
{
}

void CGdpSession::Init(const string &id, OnReceivePacketCallback onReceivePacket, OnCloseCallback onClose)
{
	_id = id;
	_onReceivePacket = onReceivePacket;
	_onClose = onClose;
}

const string &CGdpSession::GetId()
{
	return _id;
}

boost::asio::ip::tcp::socket &CGdpSession::GetSocket()
{
	return _socket;
}

const boost::asio::ip::tcp::endpoint &CGdpSession::GetRemote()
{
	return _remote;
}

bool CGdpSession::Start()
{
	if(!_socket.is_open())
	{
		LogAssert("gdp_session_not_open", "id="<<_id);
		Close();
		return false;
	}
	_connected = true;
	boost::system::error_code ec;
	_remote = _socket.remote_endpoint(ec);
	LogInfo("gdp_session_start", "id="<<_id<<",client="<<_remote.address().to_string()<<":"<<_remote.port());
	_ios.post(boost::bind(&CGdpSession::ReceiveHead, shared_from_this()));
	return true;
}

void CGdpSession::Close()
{
	_ios.post(boost::bind(&CGdpSession::AsyncClose, shared_from_this()));
}

void CGdpSession::Send(const byte *pData ,size_t length)
{
	shared_ptr<buffer_t> pBuffer(new buffer_t(pData, pData + length));
	_ios.post(boost::bind(&CGdpSession::AsyncSend, shared_from_this(), pBuffer));
}

void CGdpSession::AsyncSend(shared_ptr<buffer_t> pBuffer)
{
	uint32 length = (uint)pBuffer->size();
	_pendingBuffer.insert(_pendingBuffer.end(), (byte *)&length, ((byte *)&length) + sizeof(length));
	_pendingBuffer.insert(_pendingBuffer.end(), pBuffer->begin(), pBuffer->end());
	if(!_connected)
	{
		LogError("gdp_session_drop_data", "id="<<_id<<
			",client="<<_remote.address().to_string()<<":"<<_remote.port()<<",length="<<pBuffer->size());
		return;
	}
	if(_sendingBuffer.empty())
	{
		_sendingBuffer.swap(_pendingBuffer);
		boost::asio::async_write(_socket, boost::asio::buffer(_sendingBuffer),
			boost::bind(&CGdpSession::OnSend, shared_from_this(), boost::asio::placeholders::error));
	}
}

void CGdpSession::OnSend(const boost::system::error_code &error)
{
	if(error)
	{
		if(error != boost::asio::error::operation_aborted)
		{
			LogError("gdp_session_send_error", "id="<<_id<<",client="<<_remote.address().to_string()<<":"<<_remote.port()<<
				",error="<<error.category().name()<<"("<<error.value()<<"):"<<error.message()<<",length="<<_sendingBuffer.size());
			Close();
		}
		return;
	}
	_sendingBuffer.clear();
	if(!_pendingBuffer.empty())
	{
		_sendingBuffer.swap(_pendingBuffer);
		boost::asio::async_write(_socket, boost::asio::buffer(_sendingBuffer),
			boost::bind(&CGdpSession::OnSend, shared_from_this(), boost::asio::placeholders::error));
	}
}

void CGdpSession::ReceiveHead()
{
	boost::asio::async_read(
		_socket, boost::asio::buffer(_recvBuffer, sizeof(uint32)),
		boost::bind(&CGdpSession::OnReceiveHead,  shared_from_this(), 
			boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void CGdpSession::OnReceiveHead(const boost::system::error_code &error, size_t length)
{
	if(error)
	{
		if(error != boost::asio::error::operation_aborted)
		{
			if(error == boost::asio::error::eof)
			{
				LogInfo("gdp_session_client_close", "id="<<_id<<",client="<<_remote.address().to_string()<<":"<<_remote.port());
			}
			else
			{
				LogError("gdp_session_receive_error", "id="<<_id<<",client="<<_remote.address().to_string()<<":"<<_remote.port()<<
					",error="<<error.category().name()<<"("<<error.value()<<"):"<<error.message());
			}
			Close();
		}
		return;
	}
	if(length != sizeof(uint32))
	{
		LogAssert("gdp_session_recv_head_length_error", "id="<<_id<<
			",client="<<_remote.address().to_string()<<":"<<_remote.port()<<",length="<<length);
		Close();
		return;
	}
	uint32 packetSize;
	memcpy(&packetSize, _recvBuffer, sizeof(uint32));
	if(packetSize > MAX_PACKET_SIZE)
	{
		LogAssert("gdp_session_receive_packet_size_overflow", "id="<<_id<<",size="<<packetSize);
		Close();
		return;
	}
	boost::asio::async_read(
		_socket, boost::asio::buffer(_recvBuffer, packetSize),
		boost::bind(&CGdpSession::OnReceiveBody,  shared_from_this(), 
			boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void CGdpSession::OnReceiveBody(const boost::system::error_code &error, size_t length)
{
	if(error)
	{
		if(error != boost::asio::error::operation_aborted)
		{
			LogError("gdp_session_receive_error", "id="<<_id<<",client="<<_remote.address().to_string()<<":"<<_remote.port()<<
				",error="<<error.category().name()<<"("<<error.value()<<"):"<<error.message());
			Close();
		}
		return;
	}
	if(_onReceivePacket)
	{
		if(!_onReceivePacket(shared_from_this(), _recvBuffer, (uint32)length))
		{
			LogError("gdp_session_process_error_close", "id="<<_id<<",client="<<_remote.address().to_string()<<":"<<_remote.port());
			Close();
			return;
		}
	}
	ReceiveHead();
}

void CGdpSession::AsyncClose()
{
	LogInfo("gdp_session_close", "id="<<_id<<",client="<<_remote.address().to_string()<<":"<<_remote.port());
	if(_socket.is_open())
	{
		_connected = false;
		if(!_sendingBuffer.empty())
		{
			LogWarn("tcp_session_close_with_sending_data", "id="<<_id<<",length="<<_sendingBuffer.size());
		}
		boost::system::error_code ec;
		_socket.close(ec);
	}
	if(_onClose)
	{
		_onClose(shared_from_this());
		_onClose.clear();
	}
}
