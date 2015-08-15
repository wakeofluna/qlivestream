#include "video.h"

#include <qglobal.h>
#include <QList>
#include <QMetaType>
#include <QNetworkRequest>
#include <QString>
#include <QUrl>
#include <QUrlQuery>
#include "channel.h"
#include "profile.h"
#include "server_reply.h"

#include "../../core/logger.h"
#include "../../core/reply_base.h"
#include "../../misc.h"

namespace twitchtv3
{

bool Video::Quality::operator== (const Quality & pOther) const
{
	return (mHasVideo != pOther.mHasVideo ||
			mResolution[0] != pOther.mResolution[0] ||
			mResolution[1] != pOther.mResolution[1] ||
			mFps != pOther.mFps
			);
}

Video::Video(Channel & pChannel, QVariant pId) : IVideo(pChannel, pId)
{
	mBroadcastId = 0;
}

Video::~Video()
{
}

Channel & Video::channel() const
{
	return static_cast<Channel&>(mChannel);
}

QUrl Video::videoUrl(UrlType pType)
{
	switch (pType)
	{
		case URL_PREVIEW_IMAGE:
			return mPreviewImage;

		case URL_VIDEO_DIRECT:
			break;

		case URL_VIDEO_WEBSITE:
			return mWebpage;
	}

	return QUrl();
}

void Video::updateFromVariant(QVariant pData)
{
	QVariantMap lItem = pData.toMap();
	if (mId != lItem.value("_id"))
		return;

	QMap<QString,Quality> lQualities;

	QVariantMap lResolutionMap = lItem.value("resolutions").toMap();
	for (QVariantMap::const_iterator iter = lResolutionMap.begin(), end = lResolutionMap.end(); iter != end; ++iter)
	{
		Quality & lQuality = lQualities[iter.key()];
		QString lResolution = iter.value().toString();
		QTextStream lReader(&lResolution);

		lQuality.mHasVideo = true;
		lReader >> lQuality.mResolution[0];
		lReader.read(1);
		lReader >> lQuality.mResolution[1];
	}

	QVariantMap lFpsMap = lItem.value("fps").toMap();
	for (QVariantMap::const_iterator iter = lFpsMap.begin(), end = lFpsMap.end(); iter != end; ++iter)
	{
		lQualities[iter.key()].mFps = iter.value().toDouble();
	}

	bool lOk;
	bool lChanged = false;
	lChanged |= updateIfChanged(mTitle, lItem.value("title").toString());
	lChanged |= updateIfChanged(mDescription, lItem.value("description").toString());
	lChanged |= updateIfChanged(mCategory, lItem.value("game").toString());
	lChanged |= updateIfChanged(mRecorded, lItem.value("recorded_at").toDateTime());
	lChanged |= updateIfChanged(mLength, lItem.value("length").toInt(&lOk), lOk);
	lChanged |= updateIfChanged(mViews, lItem.value("views").toInt(&lOk), lOk);

	lChanged |= updateIfChanged(mBroadcastId, lItem.value("broadcast_id"));
	lChanged |= updateIfChanged(mPreviewImage, lItem.value("preview").toUrl());
	lChanged |= updateIfChanged(mWebpage, lItem.value("url").toUrl());
	lChanged |= updateIfChanged(mTags, lItem.value("tag_list").toStringList());
	lChanged |= updateIfChanged(mQualities, lQualities);

	if (lChanged)
		emit infoUpdated();
}


} // namespace twitchtv3
