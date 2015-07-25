#include "config.h"
#include "channel_object.h"
#include "profile.h"

#include <QCryptographicHash>

ChannelObject::ChannelObject(Profile & pProfile) : QObject(&pProfile), mProfile(pProfile)
{
	mNumViews = -1;
	mNumFollowers = -1;
	mNumSubscribers = -1;
	mDelay = -1;
	mSelf = false;
	mEditor = false;
	mPartnered = false;
	mMature = false;
	mFollowed = false;

	mNumViewers = -1;
	mFPS = 0;
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
		setStats(pOther.mNumFollowers, pOther.mNumViews, pOther.mNumViewers);

	return ok;
}

void ChannelObject::setStats(int pFollowers, int pViews, int pViewers)
{
	if (mNumFollowers != pFollowers || mNumViews != pViews || mNumViewers != pViewers)
	{
		mNumViews = pViews;
		mNumFollowers = pFollowers;
		mNumViewers = pViewers;
		emit statsChanged();
	}
}
