#ifndef __WEBSOCKET_SCENE_H__
#define __WEBSOCKET_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "NetworkWebSocket.h"
#include <ctime>

USING_NS_CC;

class WebSocketScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();

    CREATE_FUNC(WebSocketScene);

private:
	ui::TextField* _urlTextField = nullptr;
	ui::TextField* _messageTextField = nullptr;

	ui::Button* _sendButton = nullptr;
	ui::Button* _connectButton = nullptr;
	ui::Button* _disconnectButton = nullptr;
	
	ui::ListView* _messagesListView = nullptr;
	NetworkWebSocket* _webSocket = nullptr;

	void initUiEvents();
	void initButtonsState();

	void onConnectButtonClick(Ref* ref);
	void onDisconnectButtonClick(Ref* ref);
	void onSendButtonClick(Ref* ref);

	void onUrlTextFieldChanged(Ref* ref, ui::TextField::EventType eventType);
	void onMessageTextFieldChanged(Ref* ref, ui::TextField::EventType eventType);

	void refreshConnectButtonState();
	void refreshDisconnectButtonState();
	void refreshSendButtonState();
	void refreshButtonsState();

	void addMessageToListView(string message);
	void addMessageToListView(string message, Color4B textColor);

	std::string getCurrentTime(const char* format = "%c");

	void onConnectionOpened();
	void onMessageReceived(string message);
	void onConnectionClosed();
	void onConnectionError(const cocos2d::network::WebSocket::ErrorCode &error);
};

#endif
