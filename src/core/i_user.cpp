#include "i_user.h"
#include "i_profile.h"


IUser::IUser(IProfile & pProfile) : QObject(&pProfile), mProfile(pProfile)
{
}

IUser::~IUser()
{
}

bool IUser::isSelf() const
{
	return mName == mProfile.account();
}

IChannel * IUser::channel(bool pCreate)
{
	if (!mChannel && pCreate)
		mChannel.reset(newChannel());

	return mChannel.get();
}

IChannelUser * IUser::getChannelUser(IChannel & pChannel, bool pCreate)
{
	IChannelUser * lCU = nullptr;

	auto i = mChannelUsers.find(&pChannel);
	if (i != mChannelUsers.end())
	{
		lCU = *i;
	}
	else if (pCreate)
	{
		lCU = newChannelUser(pChannel);
		mChannelUsers.insert(&pChannel, lCU);
	}

	return lCU;
}
