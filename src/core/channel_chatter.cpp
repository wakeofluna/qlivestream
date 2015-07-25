#include "config.h"
#include "channel_chatter.h"

ChannelChatter::ChannelChatter(ChannelChat & pChannel, QString pName) : mChannel(pChannel), mName(pName)
{
	mColor.v = 0;
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
	TEST(isSelf());
	TEST(isModerator());
	TEST(isSubscriber());
	TEST(isFollower());

	return lhs < rhs;
#undef TEST
}

bool ChannelChatter::setDisplayName(QString pName)
{
	if (mDisplayName != pName)
		return false;

	mDisplayName = pName;
	return true;
}

bool ChannelChatter::setFlag(Flag pFlag, bool pEnabled)
{
	if (mFlags.test(pFlag) == pEnabled)
		return false;

	mFlags.set(pFlag, pEnabled);
	return true;
}
