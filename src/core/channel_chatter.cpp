#include "config.h"
#include "channel_chatter.h"

ChannelChatter::ChannelChatter(ChannelChat & pChannel, QString pName) : mChannel(pChannel), mName(pName)
{
	mIsSelf = false;
	mIsFollower = false;
	mIsSubscriber = false;
	mIsModerator = false;
	mIsOwner = false;
}

ChannelChatter::~ChannelChatter()
{
}


bool ChannelChatter::sortStatus(ChannelChatter const& lhs, ChannelChatter const& rhs)
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
	TEST(isModerator());
	TEST(isSubscriber());
	TEST(isFollower());

	return lhs < rhs;
#undef TEST
}