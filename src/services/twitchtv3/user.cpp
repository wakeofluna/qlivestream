#include "user.h"
#include "channel.h"
#include "channel_user.h"
#include "profile.h"

#include "../../misc.h"


namespace twitchtv3
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
	if (updateIfChanged<Flag>(mFlags, pFlag, pEnabled))
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

} // namespace twitchtv3
