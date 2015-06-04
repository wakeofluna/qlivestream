#include "config.h"
#include "category_object.h"

#include <QCryptographicHash>

CategoryObject::CategoryObject()
{
	mNumChannels = -1;
	mNumViewers = -1;
	mFollowed = false;
}

CategoryObject::~CategoryObject()
{
}

QString CategoryObject::logoCacheString() const
{
	return logoCacheString(mName);
}

QString CategoryObject::logoCacheString(QString pTag)
{
	return QCryptographicHash::hash(QString("c:%1").arg(pTag).toUtf8(), QCryptographicHash::Md5).toHex();
}

bool CategoryObject::isValid() const
{
	return mNumChannels != -1 && mNumViewers != -1;
}

void CategoryObject::clear()
{
	setStats(-1, -1);
}

bool CategoryObject::updateFrom(CategoryObject const & pOther)
{
	bool ok = true;

	ok &= pOther.mId == mId;
	if (ok)
		setStats(pOther.mNumChannels, pOther.mNumViewers);

	return ok;
}

bool CategoryObject::operator< (CategoryObject const & pOther) const
{
	if (mNumViewers != pOther.mNumViewers)
		return mNumViewers > pOther.mNumViewers;
	if (mNumChannels != pOther.mNumChannels)
		return mNumChannels > pOther.mNumChannels;
	return mName < pOther.mName;
}

void CategoryObject::setStats(int pChannels, int pViewers)
{
	if (pChannels != mNumChannels || pViewers != mNumViewers)
	{
		mNumChannels = pChannels;
		mNumViewers = pViewers;
		emit statsChanged();
	}
}
