#ifndef SERVICES_TWITCHTV_VIDEO_DOWNLOADER_H_
#define SERVICES_TWITCHTV_VIDEO_DOWNLOADER_H_

#include <QList>
#include <QString>
#include <QUrl>
class QNetworkReply;

#include "../../core/i_downloader.h"

namespace twitchtv
{

class Video;

class VideoDownloader : public IDownloader
{
public:
	struct Entry
	{
		inline Entry() : mLength(0) {}
		double mLength;
		QString mLocation;
	};

public:
	explicit VideoDownloader(Video & pVideo);
	~VideoDownloader();

	QString suggestedFolder() const override;
	QString suggestedFilename(int pChunkNr) const override;
	void initialize() override;
	int numChunks() const override;
	void setCurrentChunk(int pChunkNr) override;
	void startDownload() override;
	void cancelDownload() override;

private slots:
	void onReadyRead();
	void onFinished();

private:
	QUrl mUrl;
	QString mBaseUrl;
	QString mOwner;
	QString mVideoId;
	QString mFilename;
	QString mTitle;
	QList<Entry> mChunks;
	QNetworkReply * mCurrentReply;
};

} // namespace twitchtv

#endif // SERVICES_TWITCHTV_VIDEO_DOWNLOADER_H_
