#ifndef SERVICES_TWITCHTV3_CHAT_CHANNEL_H_
#define SERVICES_TWITCHTV3_CHAT_CHANNEL_H_

#include "core/channel_chat.h"
#include "chat_server.h"

namespace twitchtv3
{

class Channel;
class ChatServer;
class ChatChannel : public ::ChannelChat
{
Q_OBJECT

public:
	ChatChannel(Channel & pChannel, ChatServer::Ptr pServer);
	~ChatChannel();

	Channel & channel() const;

	void connectToChat() override;
	void disconnectFromChat(QString pMessage) override;
	void sendMessage(QString pMessage) override;

private:
	friend class ChatServer;

	ChatServer::Ptr mServer;
};

} // namespace twitchtv3

#endif // SERVICES_TWITCHTV3_CHAT_CHANNEL_H_
