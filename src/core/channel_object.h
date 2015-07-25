#ifndef CORE_CHANNEL_OBJECT_H_
#define CORE_CHANNEL_OBJECT_H_

#include "config.h"
#include <QObject>
#include <QDateTime>
#include <QString>
#include <QUrl>
#include <QVariant>

class ChannelChat;
class Profile;

class COREDLL ChannelObject : public QObject
{
Q_OBJECT

public:
	enum UrlType
	{
		URL_CHANNEL = 0,
		URL_STREAM_WEBSITE,
		URL_STREAM_DIRECT,
	};

public:
	explicit ChannelObject(Profile & pProfile);
	virtual ~ChannelObject();

	inline Profile & profile() const { return mProfile; }

	inline QVariant id() const { return mId; }
	inline QString name() const { return mName; }
	inline QString displayName() const { return mDisplayName; }
	inline QString status() const { return mStatus; }
	inline QUrl logoUrl() const { return mLogoUrl; }
	inline int numViewers() const { return mNumViewers; }
	inline int numViews() const { return mNumViews; }
	inline int numFollowers() const { return mNumFollowers; }
	inline int numSubscribers() const { return mNumSubscribers; }
	inline int delay() const { return mDelay; }
	inline bool isSelf() const { return mSelf; }
	inline bool isEditor() const { return mEditor; }
	inline bool isMature() const { return mMature; }
	inline bool isPartnered() const { return mPartnered; }
	inline bool isFollowed() const { return mFollowed; }
	inline QString category() const { return mCategory; }
	inline bool isOnline() const { return mNumViewers >= 0; }

	virtual QString logoCacheString() const;
	static QString logoCacheString(QString pTag);

	virtual ChannelChat * chat() = 0;

	void clear();
	bool updateFrom(ChannelObject const & pOther);

public slots:
	virtual void requestUpdate() = 0;
	virtual void updateStreamSettings(QString pTitle, QString pCategory, bool pMature, int pDelay) = 0;
	virtual QUrl getStreamUrl(UrlType pType) = 0;

signals:
	void statsChanged();

protected:
	void setStats(int pFollowers, int pViews, int pViewers);

	Profile & mProfile;
	QVariant mId;
	QString  mName;
	QString  mDisplayName;
	QString  mStatus;
	QUrl     mLogoUrl;
	int      mNumViews;
	int      mNumFollowers;
	int      mNumSubscribers;
	int      mDelay;
	bool     mSelf;
	bool     mEditor;
	bool     mPartnered;
	bool     mMature;
	bool     mFollowed;
	QString  mCategory;

	QDateTime mOnlineSince;
	int       mNumViewers;
	double    mFPS;
};

#endif // CORE_CHANNEL_OBJECT_H_
