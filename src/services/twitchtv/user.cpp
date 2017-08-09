#include "user.h"
#include "channel.h"
#include "channel_user.h"
#include "profile.h"

#include "../../misc.h"


namespace twitchtv
{

User::User(Profile& pProfile, QString pName) : IUser(pProfile)
{
	mName = pName;
}

User::~User()
{
}

Profile & User::profile() const
{
	return static_cast<Profile&>(mProfile);
}

void User::befriend()
{
}

void User::unfriend()
{
}

void User::updateFlag(Flag pFlag, bool pEnabled)
{
	bool lChanged = false;
	lChanged |= updateIfChanged<Flag>(mFlags, pFlag, pEnabled);

	if (lChanged)
		emit infoUpdated();
}

void User::updateFromChannel(QVariantMap pMap)
{
	bool lChanged = false;
	lChanged |= updateIfChanged(mId, pMap.value("_id"));

	if (lChanged)
		emit infoUpdated();
}

void User::updateFromUserInfo(QVariantMap pMap)
{
	bool lChanged = false;
	lChanged |= updateIfChanged(mId, pMap.value("_id"));

	if (lChanged)
		emit infoUpdated();
}

IChannel * User::newChannel()
{
	return new Channel(*this);
}

IChannelUser * User::newChannelUser(IChannel & pChannel)
{
	return new ChannelUser(static_cast<Channel&>(pChannel), *this);
}

} // namespace twitchtv
