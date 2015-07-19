#include "config.h"
#include "channel_chatter.h"
#include "chat_channel.h"

namespace twitchtv3
{

ChannelChatter::ChannelChatter(ChatChannel & pChannel, QString pName) : ::ChannelChatter(pChannel, pName)
{
}

ChannelChatter::~ChannelChatter()
{
}

bool ChannelChatter::updateFromTags(ChatServer::Tags const & pTags)
{
	QString lDisplayName = pTags.value("display-name");
	bool lSubscriber = pTags.value("subscriber") == "1";
	bool lTurbo = pTags.value("turbo") == "1";
	QString lUserType = pTags.value("user-type");

	bool lChanged = false;

	if (mDisplayName != lDisplayName && !lDisplayName.isEmpty())
	{
		mDisplayName = lDisplayName;
		lChanged = true;
	}

	if (mIsSubscriber != lSubscriber)
	{
		mIsSubscriber = lSubscriber;
		lChanged = true;
	}

	if (mIsVeteran != lTurbo)
	{
		mIsVeteran = lTurbo;
		lChanged = true;
	}

	if (mUserType != lUserType && !lUserType.isEmpty())
	{
		mUserType = lUserType;
		lChanged = true;
	}

	return lChanged;
}

} // namespace twitchtv3
