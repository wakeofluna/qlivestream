#include "i_category.h"

#include <QByteArray>
#include <QCryptographicHash>
#include "i_profile.h"


ICategory::ICategory(IProfile & pProfile) : QObject(&pProfile), mProfile(pProfile)
{
	mNumChannels = -1;
	mNumViewers = -1;
	mCanRollupChannels = true;
}

ICategory::~ICategory()
{
}

QString ICategory::logoCacheString() const
{
	return logoCacheString(mName);
}

QString ICategory::logoCacheString(QString pTag)
{
	return QCryptographicHash::hash(QString("c:%1").arg(pTag).toUtf8(), QCryptographicHash::Md5).toHex();
}

bool ICategory::operator< (ICategory const & pOther) const
{
	if (mNumViewers != pOther.mNumViewers)
		return mNumViewers > pOther.mNumViewers;
	if (mNumChannels != pOther.mNumChannels)
		return mNumChannels > pOther.mNumChannels;

	return mName < pOther.mName;
}

void ICategory::resetChannels()
{
	mChannels.clear();
	mCanRollupChannels = true;
	rollupChannels();
}

