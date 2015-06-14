#include "config.h"
#include "channel_chat.h"

ChannelChat::ChannelChat(QObject * parent) : QObject(parent)
{
	mIsConnected = false;
}

ChannelChat::~ChannelChat()
{
}
