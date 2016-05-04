#pragma once

#include <functional>
#include "network/WebSocket.h"

using namespace cocos2d;
using namespace std;

class NetworkWebSocket : public network::WebSocket::Delegate
{
public:
	typedef function<void(string message)> MessageRecivedCallBack;
	typedef function<void()> ConnectionClosedCallBack;
	typedef function<void(const cocos2d::network::WebSocket::ErrorCode &error)> ErrorOccurredCallBack;
	typedef function<void()> ConnectionOpenedCallBack;

	static NetworkWebSocket* create(string url);

	NetworkWebSocket(string url);
	~NetworkWebSocket();

	string getUrl();
	network::WebSocket* getSocket();
	network::WebSocket::State getState();

	bool connect();
	bool close();

	bool send(string message);

	void setOnMessageReceivedCallBack(MessageRecivedCallBack callBack);
	void setOnConnectionClosedCallBack(ConnectionClosedCallBack callBack);
	void setOnErrorOccurredCallBack(ErrorOccurredCallBack callBack);
	void setOnConnectionOpenedCallBack(ConnectionOpenedCallBack callBack);
private:
	network::WebSocket* _socket = nullptr;
	string _url = "";
	vector<string> _pendingMessages;

	virtual void onOpen(cocos2d::network::WebSocket* ws);
	virtual void onMessage(cocos2d::network::WebSocket* ws, const cocos2d::network::WebSocket::Data& data);
	virtual void onError(cocos2d::network::WebSocket* ws, const cocos2d::network::WebSocket::ErrorCode& error);
	virtual void onClose(cocos2d::network::WebSocket* ws);

	MessageRecivedCallBack _onMessageReceivedCallBack;
	ConnectionClosedCallBack _onConnectionClosedCallBack;
	ErrorOccurredCallBack _onErrorOccurredCallBack;
	ConnectionOpenedCallBack _onConnectionOpenedCallBack;
};