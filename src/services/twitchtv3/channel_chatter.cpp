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
	Color lColor = parseColor(pTags.value("color"));

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

	if (mColor != lColor && lColor.isValid())
	{
		mColor = lColor;
		lChanged = true;
	}

	return lChanged;
}

ChannelChatter::Color ChannelChatter::parseColor(QString pInput)
{
	Color lColor;

	if (pInput.length() != 7)
		return lColor;

	if (pInput[0] != '#')
		return lColor;

	bool ok[3];
	lColor.r = pInput.mid(1, 2).toInt(&ok[0], 16);
	lColor.g = pInput.mid(3, 2).toInt(&ok[1], 16);
	lColor.b = pInput.mid(5, 2).toInt(&ok[2], 16);
	lColor.a = 255;

	if (!ok[0] || !ok[1] || !ok[2])
		lColor.a = 0;
	else
		lColor.a = 255;

	return lColor;
}

} // namespace twitchtv3
