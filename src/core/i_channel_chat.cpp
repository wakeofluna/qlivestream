#include "i_channel_chat.h"
#include "i_channel.h"


IChannelChat::IChannelChat(IChannel & pChannel) : QObject(&pChannel), mChannel(pChannel)
{
	mState = NONE;
}

IChannelChat::~IChannelChat()
{
	qDeleteAll(mChatters);
}

bool IChannelChat::hasChatter(IChannelUser const & pChatter)
{
	return mChatters.contains(const_cast<IChannelUser*>(&pChatter));
}

void IChannelChat::setState(State pState)
{
	if (mState != pState)
	{
		mState = pState;
		emit stateChanged();
	}
}
