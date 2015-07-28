#include "channel_user.h"

#include <QChar>
#include <QObject>
#include "channel.h"
#include "profile.h"
#include "user.h"

#include "../../core/i_user.h"
#include "../../misc.h"

namespace twitchtv3
{

ChannelUser::ChannelUser(Channel & pChannel, User & pUser) : IChannelUser(pChannel, pUser)
{
	QObject::connect(&pUser, &IUser::infoUpdated, this, &IChannelUser::infoUpdated);
}

ChannelUser::~ChannelUser()
{
}

Profile & ChannelUser::profile() const
{
	return user().profile();
}

Channel & ChannelUser::channel() const
{
	return static_cast<Channel&>(mChannel);
}

User & ChannelUser::user() const
{
	return static_cast<User&>(mUser);
}

void ChannelUser::updateFromTags(ChatServer::Tags const & pTags)
{
	QString lDisplayName = pTags.value("display-name");
	bool lSubscriber = pTags.value("subscriber") == "1";
	bool lTurbo = pTags.value("turbo") == "1";
	QString lUserType = pTags.value("user-type");
	Color lColor = parseColor(pTags.value("color"));

	user().updateFlag(IUser::Flag::VETERAN, lTurbo);

	bool lChanged = false;
	lChanged |= updateIfChanged(mDisplayName, lDisplayName, !lDisplayName.isEmpty());
	lChanged |= updateIfChanged(mUserType, lUserType, !lUserType.isEmpty());
	lChanged |= updateIfChanged(mColor, lColor, lColor.isValid());
	lChanged |= updateIfChanged<Flag>(mFlags, Flag::SUBSCRIBER, lSubscriber);
	lChanged |= updateIfChanged<Flag>(mFlags, Flag::MODERATOR, lUserType == "mod");

	if (lChanged)
		emit infoUpdated();
}

void ChannelUser::updateFlag(Flag pFlag, bool pEnabled)
{
	if (updateIfChanged<Flag>(mFlags, pFlag, pEnabled))
		emit infoUpdated();
}

ChannelUser::Color ChannelUser::parseColor(QString pInput)
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
