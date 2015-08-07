#include "i_channel.h"

#include <QByteArray>
#include <QCryptographicHash>
#include "i_user.h"


IChannel::IChannel(IUser & pUser) : QObject(&pUser), mUser(pUser)
{
	mCategory = nullptr;
	mNumViews = -1;
	mNumFollowers = -1;
	mNumSubscribers = -1;
	mNumViewers = -1;
	mDelay = -1;
}

IChannel::~IChannel()
{
}

bool IChannel::isSelf() const
{
	return mUser.isSelf();
}

QString IChannel::logoCacheString() const
{
	return logoCacheString(mUser.name());
}

QString IChannel::logoCacheString(QString pTag)
{
	return QCryptographicHash::hash(QString("s:%1").arg(pTag).toUtf8(), QCryptographicHash::Md5).toHex();
}

bool IChannel::operator< (IChannel const & pOther) const
{
	if (mNumViewers != pOther.mNumViewers)
		return mNumViewers > pOther.mNumViewers;

	if (mOnlineSince != pOther.mOnlineSince)
		return mOnlineSince < pOther.mOnlineSince;

	return mDisplayName < pOther.mDisplayName;
}

void IChannel::follow()
{
	setFollowed(true);
}

void IChannel::unfollow()
{
	setFollowed(false);
}

