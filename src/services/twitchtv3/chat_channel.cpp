#include "config.h"
#include "chat_channel.h"
#include "channel.h"
#include "chat_server.h"

namespace twitchtv3
{

ChatChannel::ChatChannel(Channel & pChannel, ChatServer::Ptr pServer) : ChannelChat(pChannel), mServer(pServer)
{
}

ChatChannel::~ChatChannel()
{
	mServer->removeChannel(this);
}

Channel & ChatChannel::channel() const
{
	return static_cast<Channel&>(mChannel);
}

void ChatChannel::connectToChat()
{
	mServer->joinChannel(this);
}

void ChatChannel::disconnectFromChat(QString pMessage)
{
	mServer->leaveChannel(this, pMessage);
}

void ChatChannel::sendMessage(QString pMessage)
{
}

} // namespace twitchtv3
