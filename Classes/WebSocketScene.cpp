#include "WebSocketScene.h"

using namespace cocostudio::timeline;

Scene* WebSocketScene::createScene()
{    
    auto scene = Scene::create();   
    auto layer = WebSocketScene::create();

    scene->addChild(layer);

    return scene;
}

bool WebSocketScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    auto rootNode = CSLoader::createNode("MainScene.csb");
    addChild(rootNode);
	auto mainPanel = rootNode->getChildByName("MainPanel");

	auto connectionSettingsPanel = mainPanel->getChildByName("ConnectionSettingsPanel");
	this->_urlTextField = static_cast<ui::TextField*>(connectionSettingsPanel->getChildByName("UrlTextField"));
	this->_connectButton = static_cast<cocos2d::ui::Button*>(connectionSettingsPanel->getChildByName("ConnectButton"));
	this->_disconnectButton = static_cast<cocos2d::ui::Button*>(connectionSettingsPanel->getChildByName("DisconnectButton"));

	auto messagesPanel = mainPanel->getChildByName("MessagesPanel");
	this->_messageTextField = static_cast<ui::TextField*>(messagesPanel->getChildByName("MessageTextField"));
	this->_sendButton = static_cast<cocos2d::ui::Button*>(messagesPanel->getChildByName("SendButton"));	
	
	this->_messagesListView = static_cast<cocos2d::ui::ListView*>(messagesPanel->getChildByName("MessagesListView"));
	this->_messagesListView->setItemsMargin(5);
	this->_messagesListView->setClippingEnabled(true);

	this->initUiEvents();
	this->initButtonsState();

    return true;
}

void WebSocketScene::initUiEvents()
{
	this->_connectButton->addClickEventListener(CC_CALLBACK_1(WebSocketScene::onConnectButtonClick, this));
	this->_disconnectButton->addClickEventListener(CC_CALLBACK_1(WebSocketScene::onDisconnectButtonClick, this));
	this->_sendButton->addClickEventListener(CC_CALLBACK_1(WebSocketScene::onSendButtonClick, this));

	this->_urlTextField->addEventListener(CC_CALLBACK_2(WebSocketScene::onUrlTextFieldChanged, this));
	this->_messageTextField->addEventListener(CC_CALLBACK_2(WebSocketScene::onMessageTextFieldChanged, this));
}

void WebSocketScene::initButtonsState()
{
	// The buttons will be enabled by events
	this->_connectButton->setEnabled(false);
	this->_disconnectButton->setEnabled(false);
	this->_sendButton->setEnabled(false);
}

void WebSocketScene::onConnectButtonClick(Ref * ref)
{
	this->_webSocket = NetworkWebSocket::create(this->_urlTextField->getString());
	if (this->_webSocket->connect()) {
		this->_webSocket->setOnConnectionOpenedCallBack(CC_CALLBACK_0(WebSocketScene::onConnectionOpened, this));
		this->_webSocket->setOnMessageReceivedCallBack(CC_CALLBACK_1(WebSocketScene::onMessageReceived, this));
		this->_webSocket->setOnConnectionClosedCallBack(CC_CALLBACK_0(WebSocketScene::onConnectionClosed, this));
		this->_webSocket->setOnErrorOccurredCallBack(CC_CALLBACK_1(WebSocketScene::onConnectionError, this));
	}
}

void WebSocketScene::onDisconnectButtonClick(Ref * ref)
{
	this->_webSocket->close();
	CC_SAFE_DELETE(this->_webSocket);
}

void WebSocketScene::onSendButtonClick(Ref * ref)
{
	auto value = this->_messageTextField->getString();
	this->_webSocket->send(value);
	this->addMessageToListView("Send : " + value, Color4B::BLUE);
	this->_messageTextField->setString("");

	this->refreshSendButtonState();
}

void WebSocketScene::onUrlTextFieldChanged(Ref * ref, ui::TextField::EventType eventType)
{
	if (eventType == ui::TextField::EventType::INSERT_TEXT || eventType == ui::TextField::EventType::DELETE_BACKWARD) {
		this->refreshConnectButtonState();
	}
}

void WebSocketScene::onMessageTextFieldChanged(Ref * ref, ui::TextField::EventType eventType)
{
	if (eventType == ui::TextField::EventType::INSERT_TEXT || eventType == ui::TextField::EventType::DELETE_BACKWARD) {
		this->refreshSendButtonState();
	}
}

void WebSocketScene::refreshConnectButtonState()
{
	this->_connectButton->setEnabled((this->_webSocket == nullptr || (this->_webSocket->getState() == network::WebSocket::State::CLOSED || this->_webSocket->getState() == network::WebSocket::State::CLOSING)) && !this->_urlTextField->getString().empty());
}

void WebSocketScene::refreshDisconnectButtonState()
{
	this->_disconnectButton->setEnabled((this->_webSocket != nullptr && this->_webSocket->getState() == network::WebSocket::State::OPEN));
}

void WebSocketScene::refreshSendButtonState()
{
	this->_sendButton->setEnabled((this->_webSocket != nullptr && this->_webSocket->getState() == network::WebSocket::State::OPEN) && !this->_messageTextField->getString().empty());
}

void WebSocketScene::refreshButtonsState()
{
	this->refreshConnectButtonState();
	this->refreshDisconnectButtonState();
	this->refreshSendButtonState();
}

void WebSocketScene::addMessageToListView(string message)
{
	this->addMessageToListView(message, Color4B::BLACK);
}

void WebSocketScene::addMessageToListView(string message, Color4B textColor)
{
	auto widget = ui::Text::create(this->getCurrentTime() + string(" - ") + message, "", 12);
	widget->setTextColor(textColor);
	this->_messagesListView->pushBackCustomItem(widget);	
	this->_messagesListView->scrollToBottom(0, false);
}

std::string WebSocketScene::getCurrentTime(const char* format)
{
	std::time_t t = std::time(0);
	char cstr[128];
	std::strftime(cstr, sizeof(cstr), format, std::localtime(&t));
	return cstr;
}

void WebSocketScene::onConnectionOpened()
{
	this->addMessageToListView("INFO : Connection opened", Color4B(150, 150, 150, 255));

	this->refreshButtonsState();
}

void WebSocketScene::onMessageReceived(string message)
{
	this->addMessageToListView("Receive : " + message, Color4B::BLACK);	
}

void WebSocketScene::onConnectionClosed()
{
	this->addMessageToListView("INFO : Connection closed", Color4B(150, 150, 150, 255));

	this->refreshButtonsState();
}

void WebSocketScene::onConnectionError(const cocos2d::network::WebSocket::ErrorCode & error)
{
	string errorCodeValue = "";
	switch (error)
	{
	case cocos2d::network::WebSocket::ErrorCode::CONNECTION_FAILURE:
		errorCodeValue = "CONNECTION_FAILURE";
		break;
	case cocos2d::network::WebSocket::ErrorCode::TIME_OUT:
		errorCodeValue = "TIME_OUT";
		break;
	case cocos2d::network::WebSocket::ErrorCode::UNKNOWN:
		errorCodeValue = "UNKNOWN";
		break;	
	}

	this->addMessageToListView("ERROR : " + errorCodeValue, Color4B::RED);

	this->refreshButtonsState();
}
