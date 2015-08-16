#ifndef CORE_I_VIDEO_H_
#define CORE_I_VIDEO_H_

#include <QList>
#include <QString>
#include <QUrl>
#include <QVariant>
#include "i_channel.h"

#include "../config.h"

class IDownloader;
class IProfile;

class COREDLL IVideo : public QObject
{
Q_OBJECT

public:
	enum UrlType
	{
		URL_PREVIEW_IMAGE,
		URL_VIDEO_WEBSITE,
		URL_VIDEO_DIRECT,
	};

public:
	explicit IVideo(IChannel & pChannel, QVariant pId);
	virtual ~IVideo();

	inline IChannel const & channel() const { return mChannel; }
	inline IChannel & channel() { return mChannel; }

	inline QVariant id() const { return mId; }
	inline QString title() const { return mTitle; }
	inline QString description() const { return mDescription; }
	inline QString category() const { return mCategory; }
	inline QDateTime recorded() const { return mRecorded; }
	inline int length() const { return mLength; }
	inline int views() const { return mViews; }

	virtual QUrl videoUrl(UrlType pType) = 0;
	virtual IDownloader * downloader() = 0;

	bool operator< (IVideo const & pOther) const;

signals:
	void infoUpdated();

protected:
	IChannel & mChannel;
	QVariant mId;
	QString mTitle;
	QString mDescription;
	QString mCategory;
	QDateTime mRecorded;
	int mLength;
	int mViews;
};

#endif // CORE_I_VIDEO_H_
