#include "config.h"
#include "channel_chat.h"
#include "core/channel_object.h"

ChannelChat::ChannelChat(ChannelObject & pChannel) : QObject(&pChannel), mChannel(pChannel)
{
	mState = NONE;
}

ChannelChat::~ChannelChat()
{
}

QString ChannelChat::name() const
{
	return mChannel.name().toLower();
}

void ChannelChat::setState(State pNewState)
{
	if (mState != pNewState)
	{
		mState = pNewState;
		emit chatStateChanged();
	}
}
