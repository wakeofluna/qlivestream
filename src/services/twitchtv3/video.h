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
class Video : public IVideo
{
public:
	struct Quality
	{
		bool mHasVideo = false;
		int mResolution[2] = { -1, -1 };
		float mFps = 0;
		bool operator== (const Quality & pOther) const;
	};

public:
	Video(Channel & pChannel, QVariant pId);
	~Video();

	Channel & channel() const;

	QUrl videoUrl(UrlType pType) override;

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
