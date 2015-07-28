#include "i_channel_user.h"
#include "i_channel.h"
#include "i_channel_chat.h"
#include "i_profile.h"
#include "i_user.h"


IChannelUser::IChannelUser(IChannel & pChannel, IUser & pUser) : mChannel(pChannel), mUser(pUser)
{
	mDisplayName = mUser.name();
	mColor.v = 0;
}

IChannelUser::~IChannelUser()
{
	// TODO disconnect from both user and channel. Templatize
}

bool IChannelUser::isSelf() const
{
	return &mChannel.owner().profile().self() == &mUser;
}

bool IChannelUser::isOwner() const
{
	return mUser.channel(false) == &mChannel;
}

bool IChannelUser::sortStatus(IChannelUser const& lhs, IChannelUser const& rhs)
{
#define TEST(x) \
	if (lhs.x) \
	{ \
		if (!rhs.x) return true; \
		return lhs < rhs; \
	} \
	else if (rhs.x) \
		return false

	TEST(isOwner());
	TEST(mUser.isSelf());
	TEST(isModerator());
	TEST(isSubscriber());
	TEST(isFollower());

	return lhs < rhs;
#undef TEST
}

void IChannelUser::onInfoUpdated()
{
	IChannelChat * lChat = mChannel.chat();
	if (lChat->hasChatter(*this))
		emit lChat->chatterChanged(*this);
}

