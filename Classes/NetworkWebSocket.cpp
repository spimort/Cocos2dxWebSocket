#include "NetworkWebSocket.h"

NetworkWebSocket * NetworkWebSocket::create(string url)
{
	auto socket = new NetworkWebSocket(url);

	return socket;
}

NetworkWebSocket::NetworkWebSocket(string url)
{
	this->_url = url;
}

NetworkWebSocket::~NetworkWebSocket()
{
	this->_socket->close();
}

string NetworkWebSocket::getUrl()
{
	return this->_url;
}

network::WebSocket * NetworkWebSocket::getSocket()
{
	return this->_socket;
}

network::WebSocket::State NetworkWebSocket::getState()
{
	if (this->_socket != nullptr)
		return this->_socket->getReadyState();

	return network::WebSocket::State();
}

bool NetworkWebSocket::connect()
{
	try
	{
		this->_socket = new network::WebSocket();

		return this->_socket->init(*this, this->_url);
	}
	catch (const std::exception&)
	{
		throw;
	}

	return false;
}

bool NetworkWebSocket::close()
{
	try
	{
		if (this->_socket != nullptr && this->_socket->getReadyState() == network::WebSocket::State::OPEN)
			this->_socket->close();

		return true;
	}
	catch (const std::exception&)
	{
		throw;
	}

	return false;
}

bool NetworkWebSocket::send(string message)
{
	try
	{
		if (this->_socket != nullptr) {
			if (this->_socket->getReadyState() == network::WebSocket::State::OPEN)
				this->_socket->send(message);
			else if (this->_socket->getReadyState() == network::WebSocket::State::CONNECTING)
				this->_pendingMessages.push_back(message);
		}

		return true;
	}
	catch (const std::exception&)
	{
		throw;
	}

	return false;
}

void NetworkWebSocket::setOnMessageReceivedCallBack(MessageRecivedCallBack callBack)
{
	this->_onMessageReceivedCallBack = callBack;
}

void NetworkWebSocket::setOnConnectionClosedCallBack(ConnectionClosedCallBack callBack)
{
	this->_onConnectionClosedCallBack = callBack;
}

void NetworkWebSocket::setOnErrorOccurredCallBack(ErrorOccurredCallBack callBack)
{
	this->_onErrorOccurredCallBack = callBack;
}

void NetworkWebSocket::setOnConnectionOpenedCallBack(ConnectionOpenedCallBack callBack)
{
	this->_onConnectionOpenedCallBack = callBack;
}

void NetworkWebSocket::onOpen(network::WebSocket * ws)
{
	if (this->_pendingMessages.size() > 0) {
		for (string message : _pendingMessages)
			send(message);

		this->_pendingMessages.clear();
	}

	if(_onConnectionOpenedCallBack != nullptr)
		_onConnectionOpenedCallBack();
}

void NetworkWebSocket::onMessage(network::WebSocket * ws, const network::WebSocket::Data & data)
{
	if(_onMessageReceivedCallBack != nullptr)
		_onMessageReceivedCallBack(std::string(data.bytes, data.len));
}

void NetworkWebSocket::onClose(network::WebSocket * ws)
{
	if(_onConnectionClosedCallBack != nullptr)
		_onConnectionClosedCallBack();
}

void NetworkWebSocket::onError(network::WebSocket * ws, const network::WebSocket::ErrorCode & error)
{
	if(_onErrorOccurredCallBack != nullptr)
		_onErrorOccurredCallBack(error);
}
