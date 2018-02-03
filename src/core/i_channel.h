#ifndef CORE_I_CHANNEL_H_
#define CORE_I_CHANNEL_H_

#include <QDateTime>
#include <QObject>
#include <QString>
#include <QList>
#include <QUrl>
#include <QVariant>
#include "class_bitset.h"

#include "../config.h"

class ICategory;
class IUser;
class IChannelChat;
class IProfile;
class IVideo;

class COREDLL IChannel : public QObject
{
Q_OBJECT

public:
	enum UrlType
	{
		URL_LOGO,
		URL_CHANNEL,
		URL_STREAM_WEBSITE,
		URL_STREAM_DIRECT,
	};

	struct Flag
	{
		enum Value
		{
			PARTNERED,
			MATURE,
			FOLLOWED,
			AM_EDITOR,
			INVALID
		};
		// cppcheck-suppress noExplicitConstructor
		ENUMSTRUCT(Flag, Value, INVALID);
	};
	typedef ClassBitset<Flag> Flags;

public:
	explicit IChannel(IUser & pUser);
	virtual ~IChannel();

	inline IUser & owner() const { return mUser; }

	inline QVariant id() const { return mId; }
	inline QString displayName() const { return mDisplayName; }
	inline QString title() const { return mTitle; }
	inline ICategory * category() const { return mCategory; }
	inline int numViewers() const { return mNumViewers; }
	inline int numViews() const { return mNumViews; }
	inline int numFollowers() const { return mNumFollowers; }
	inline int numSubscribers() const { return mNumSubscribers; }
	inline int delay() const { return mDelay; }

	bool isSelf() const;
	inline bool isMature() const { return mFlags.test(Flag::MATURE); }
	inline bool isPartnered() const { return mFlags.test(Flag::PARTNERED); }
	inline bool isFollowed() const { return mFlags.test(Flag::FOLLOWED); }
	inline bool amEditor() const { return mFlags.test(Flag::AM_EDITOR); }

	virtual QString logoCacheString() const;
	static QString logoCacheString(QString pTag);

	virtual IChannelChat * chat() = 0;
	virtual QUrl streamUrl(UrlType pType) = 0;

	virtual void modifyStreamSettings(QString pTitle, ICategory * pCategory, bool pMature, int pDelay) = 0;
	virtual void setFollowed(bool pFollow) = 0;

	inline bool isOnline() const { return mOnlineSince.isValid(); }
	inline QDateTime onlineSince() const { return mOnlineSince; }

	inline QList<IVideo *> videos() const { return mVideos; }

	bool operator< (IChannel const & pOther) const;

public slots:
	virtual void refresh() = 0;
	void follow();
	void unfollow();
	void resetVideos();
	virtual void rollupVideos() = 0;

signals:
	void infoUpdated();
	void videosUpdated();

protected:
	IUser     & mUser;
	QVariant  mId;
	Flags     mFlags;
	QString   mDisplayName;
	QString   mTitle;
	ICategory * mCategory;
	int       mNumViews;
	int       mNumFollowers;
	int       mNumSubscribers;
	int       mNumViewers;
	int       mDelay;

	QDateTime mFollowedSince;
	QDateTime mOnlineSince;

	bool mCanRollupVideos;
	QList<IVideo *> mVideos;
};

#endif // CORE_I_CHANNEL_H_
