#include "video_downloader.h"

#include <qglobal.h>
#include <QChar>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QVariant>
#include "channel.h"
#include "user.h"
#include "video.h"

#include "../../core/reply_text.h"

namespace twitchtv3
{

VideoDownloader::VideoDownloader(Video & pVideo) : IDownloader()
{
	mUrl = pVideo.videoUrl(IVideo::URL_VIDEO_DIRECT);

	mBaseUrl = mUrl.url(QUrl::RemoveFragment | QUrl::RemoveQuery);
	int lLastSlash = mBaseUrl.lastIndexOf('/');
	mBaseUrl = mBaseUrl.left(lLastSlash+1);

	mOwner = pVideo.channel().owner().name();
	mVideoId = pVideo.id().toString();
	mTitle = pVideo.title();
	mCurrentReply = nullptr;

	// Protect against some common evil characters XXX find a proper strategy
	mTitle.replace(' ', '_');
	mTitle.replace('\\', '-');
	mTitle.replace('*', 'x');
}

VideoDownloader::~VideoDownloader()
{
	delete mCurrentReply;
}

QString VideoDownloader::suggestedFolder() const
{
	if (mTitle.isEmpty())
		return QString("twitch-%1-%2").arg(mOwner).arg(mVideoId);
	else
		return QString("twitch-%1-%2-%3").arg(mOwner).arg(escapeFilePath(mTitle)).arg(mVideoId);
}

QString VideoDownloader::suggestedFilename(int pChunkNr) const
{
	QString lExtension;

	const Entry & lChunk = mChunks.value(pChunkNr < 0 ? 0 : pChunkNr);

	int lDot = lChunk.mLocation.lastIndexOf('.');
	if (lDot != -1)
	{
		int lEnd = lChunk.mLocation.indexOf('?', lDot);
		lExtension = lChunk.mLocation.mid(lDot, lEnd - lDot);
	}

	if (pChunkNr == -2 || (mChunks.size() <= 1 && pChunkNr == 0))
		return QString("%1%2").arg(suggestedFolder()).arg(lExtension);
	else if (pChunkNr == -1)
		return QString("%1").arg(mVideoId);
	else
		return QString("%1-part%2%3").arg(mVideoId).arg(int(pChunkNr+1), QString::number(mChunks.size()).length(), 10, QChar('0')).arg(lExtension);
}

void VideoDownloader::initialize()
{
	QNetworkRequest lRequest;
	lRequest.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
	lRequest.setUrl(mUrl);

	networkGet(lRequest, [this] (QNetworkReply & pReply)
	{
		ReplyText lReply(nullptr, pReply, "VideoChunkList");
		if (!lReply.hasError())
		{
			QStringList lLines = lReply.data();
			Entry lEntry;

			for (QString & lLine : lLines)
			{
				if (lLine.isEmpty())
					continue;

				if (lLine[0] == '#')
				{
					if (lLine.left(8) == "#EXTINF:")
						lEntry.mLength = lLine.mid(8, lLine.length() - 9).toDouble();
				}
				else
				{
					lEntry.mLocation = lLine;
					mChunks.append(lEntry);
					lEntry.mLength = 0;
				}

			}
		}

		emit initialized();
	});
}

int VideoDownloader::numChunks() const
{
	return mChunks.size();
}

void VideoDownloader::setCurrentChunk(int pChunkNr)
{
	Q_ASSERT_X(mCurrentReply == nullptr, "VideoDownloader::setCurrentChunk", "previous chunk download still busy!");

	Q_ASSERT(pChunkNr >= 0 && pChunkNr < mChunks.size());
	mCurrentChunk = pChunkNr;
	mChunkProgress = -1;
	mChunkSize = -1;
	mChunkLength = mChunks.value(pChunkNr).mLength;
}

void VideoDownloader::startDownload()
{
	Q_ASSERT_X(mCurrentReply == nullptr, "VideoDownloader::startDownload", "previous chunk download still busy!");

	const Entry & lChunk = mChunks.value(mCurrentChunk);
	if (lChunk.mLocation.isEmpty())
	{
		emit chunkCanceled();
		return;
	}

	QUrl lUrl = QString("%1%2").arg(mBaseUrl).arg(lChunk.mLocation);

	QNetworkRequest lRequest;
	lRequest.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
	lRequest.setUrl(lUrl);

	mCurrentReply = networkGetAsync(lRequest);
	QObject::connect(mCurrentReply, &QNetworkReply::finished, this, &VideoDownloader::onFinished);
	QObject::connect(mCurrentReply, &QNetworkReply::readyRead, this, &VideoDownloader::onReadyRead);
}

void VideoDownloader::cancelDownload()
{
	if (mCurrentReply != nullptr)
	{
		mCurrentReply->abort();
		mCurrentReply->deleteLater();
		mCurrentReply = nullptr;
		emit chunkCanceled();
	}
}

void VideoDownloader::onReadyRead()
{
	if (mChunkProgress == -1)
	{
		QVariant lCL = mCurrentReply->header(QNetworkRequest::ContentLengthHeader);
		bool lOk;
		int lLength = lCL.toInt(&lOk);
		if (lOk)
			mChunkSize = lLength;

		mChunkProgress = 0;
		emit chunkStarted();
	}

	QByteArray lData = mCurrentReply->readAll();
	mChunkProgress += lData.size();
	emit chunkProgress(lData);
}

void VideoDownloader::onFinished()
{
	if (mCurrentReply == nullptr)
		return;

	QByteArray lData = mCurrentReply->readAll();
	if (!lData.isEmpty())
	{
		mChunkProgress += lData.size();
		emit chunkProgress(lData);
	}

	delete mCurrentReply;
	mCurrentReply = nullptr;

	emit chunkCompleted();
}

} // namespace twitchtv3
