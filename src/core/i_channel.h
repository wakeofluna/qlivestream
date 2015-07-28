#ifndef CORE_I_CHANNEL_H_
#define CORE_I_CHANNEL_H_

#include <QDateTime>
#include <QObject>
#include <QString>
#include <QUrl>
#include <QVariant>
#include "class_bitset.h"

#include "../config.h"

class ICategory;
class IUser;
class IChannelChat;
class IProfile;

class COREDLL IChannel : public QObject
{
Q_OBJECT

public:
	enum UrlType
	{
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
	inline QUrl logoUrl() const { return mLogoUrl; }
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

	inline bool isOnline() const { return mOnlineSince.isValid(); }
	inline QDateTime onlineSince() const { return mOnlineSince; }

	bool operator< (IChannel const & pOther) const;

public slots:
	virtual void refresh() = 0;
	virtual void modifyStreamSettings(QString pTitle, ICategory * pCategory, bool pMature, int pDelay) = 0;
	virtual QUrl getStreamUrl(UrlType pType) = 0;

signals:
	void infoUpdated();

protected:
	IUser     & mUser;
	QVariant  mId;
	Flags     mFlags;
	QString   mDisplayName;
	QString   mTitle;
	QUrl      mLogoUrl;
	ICategory * mCategory;
	int       mNumViews;
	int       mNumFollowers;
	int       mNumSubscribers;
	int       mNumViewers;
	int       mDelay;

	QDateTime mFollowedSince;
	QDateTime mOnlineSince;
};

#endif // CORE_I_CHANNEL_H_
