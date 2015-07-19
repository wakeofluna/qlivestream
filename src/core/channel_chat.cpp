#include "config.h"
#include "channel_chat.h"
#include "core/channel_object.h"
#include "core/channel_chatter.h"

ChannelChat::ChannelChat(ChannelObject & pChannel) : QObject(&pChannel), mChannel(pChannel)
{
	mSelf = nullptr;
	mState = NONE;
}

ChannelChat::~ChannelChat()
{
	qDeleteAll(mChattersLeft);
	qDeleteAll(mChatters);
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
