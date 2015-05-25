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
	return QCryptographicHash::hash(mName.toUtf8(), QCryptographicHash::Md5).toHex();
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

void CategoryObject::setStats(int pChannels, int pViewers)
{
	if (pChannels != mNumChannels || pViewers != mNumViewers)
	{
		mNumChannels = pChannels;
		mNumViewers = pViewers;
		emit statsChanged();
	}
}
