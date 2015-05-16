#include "config.h"
#include "category_object.h"

#include <QCryptographicHash>

CategoryObject::CategoryObject()
{
	mNumChannels = 0;
	mNumViewers = 0;
	mFollowed = false;
}

CategoryObject::~CategoryObject()
{
}

QString CategoryObject::logoCacheString() const
{
	return QCryptographicHash::hash(mName.toUtf8(), QCryptographicHash::Md5).toHex();
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
