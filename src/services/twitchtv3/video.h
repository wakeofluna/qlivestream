#ifndef SERVICES_TWITCHTV3_VIDEO_H_
#define SERVICES_TWITCHTV3_VIDEO_H_

#include <QMap>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QVariant>

#include "../../core/i_video.h"

namespace twitchtv3
{

class Channel;
class Profile;
class Video : public IVideo
{
public:
	struct Quality
	{
		Quality() { mResolution[0] = -1; mResolution[1] = -1; }
		bool mHasVideo = false;
		int mResolution[2];
		float mFps = 0;
		bool operator== (const Quality & pOther) const;
	};

public:
	Video(Channel & pChannel, QVariant pId);
	~Video();

	Channel & channel() const;
	Profile & profile() const;

	QUrl videoUrl(UrlType pType) override;
	IDownloader * downloader() override;

	void updateFromVariant(QVariant pData);

private:
	QVariant mBroadcastId;
	QUrl mPreviewImage;
	QUrl mWebpage;
	QStringList mTags;
	QMap<QString,Quality> mQualities;
};

} // namespace twitchtv3

#endif // SERVICES_TWITCHTV3_VIDEO_H_
