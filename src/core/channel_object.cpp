#include "config.h"
#include "channel_object.h"
#include "profile.h"

#include <QCryptographicHash>

ChannelObject::ChannelObject(Profile & pProfile) : QObject(&pProfile), mProfile(pProfile)
{
	mNumViews = -1;
	mNumFollowers = -1;
	mDelay = -1;
	mSelf = false;
	mEditor = false;
	mPartnered = false;
	mMature = false;
	mFollowed = false;
}

ChannelObject::~ChannelObject()
{

}

QString ChannelObject::logoCacheString() const
{
	return logoCacheString(mName);
}

QString ChannelObject::logoCacheString(QString pTag)
{
	return QCryptographicHash::hash(QString("s:%1").arg(pTag).toUtf8(), QCryptographicHash::Md5).toHex();
}

void ChannelObject::clear()
{
}

bool ChannelObject::updateFrom(ChannelObject const & pOther)
{
	bool ok = true;

	ok &= pOther.mName == mName;
	if (ok)
		setStats(pOther.mNumFollowers, pOther.mNumViews);

	return ok;
}

void ChannelObject::setStats(int pFollowers, int pViews)
{
	if (mNumFollowers != pFollowers || mNumViews != pViews)
	{
		mNumViews = pViews;
		mNumFollowers = pFollowers;
		emit statsChanged();
	}
}
