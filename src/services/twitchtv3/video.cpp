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
#include "../../core/reply_text.h"
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

Profile & Video::profile() const
{
	return channel().profile();
}

QUrl Video::videoUrl(UrlType pType)
{
	switch (pType)
	{
		case URL_PREVIEW_IMAGE:
			return mPreviewImage;

		case URL_VIDEO_DIRECT:
		{
			QNetworkRequest lRequest;
			QUrl lUrl;
			QUrlQuery lParams;

			QString lId = id().toString().mid(1);

			lUrl = profile().apiUrl(QString("/vods/%1/access_token").arg(lId));
			lRequest = profile().serviceRequest(true);
			lRequest.setUrl(lUrl);

			ServerReply lReply(profile(), *profile().synchronisedGet(lRequest), "VideoToken");
			if (lReply.hasError())
				break;

			lParams.clear();
			lParams.addQueryItem("p", QString::number(int(double(qrand()) / RAND_MAX * 999999)));
			lParams.addQueryItem("type", "any");
			lParams.addQueryItem("allow_source", "true");
			lParams.addQueryItem("allow_audio_only", "true");
			lParams.addQueryItem("nauthsig", lReply.data().value("sig").toString());
			lParams.addQueryItem("nauth", lReply.data().value("token").toString());
			lUrl = profile().usherUrl(QString("/vod/%1").arg(lId));
			lUrl.setQuery(lParams);
			lRequest = profile().serviceRequest(false, false);
			lRequest.setUrl(lUrl);

			ReplyText lReply2(profile(), *profile().synchronisedGet(lRequest), "VideoPlaylist");
			if (lReply2.hasError())
				break;

			QStringList lPlaylist = lReply2.data();
			if (lPlaylist.isEmpty() || lPlaylist[0] != "#EXTM3U")
			{
				qCritical() << "Received playlist is not a valid M3U playlist?";
				break;
			}

			// Grab the very first valid line, should be best available quality
			for (QString & lLine : lPlaylist)
			{
				if (lLine[0] == '#')
					continue;

				QUrl lUrl(lLine);
				if (lUrl.isValid())
					return lUrl;
			}

			break;
		}

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
