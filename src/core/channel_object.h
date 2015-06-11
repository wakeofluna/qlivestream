#ifndef CORE_CHANNEL_OBJECT_H_
#define CORE_CHANNEL_OBJECT_H_

#include "config.h"
#include <QObject>
#include <QString>
#include <QUrl>
#include <QVariant>

class ChannelChat;
class COREDLL ChannelObject : public QObject
{
Q_OBJECT

public:
	ChannelObject();
	virtual ~ChannelObject();

	inline QVariant id() const { return mId; }
	inline QString name() const { return mName; }
	inline QString displayName() const { return mDisplayName; }
	inline QString status() const { return mStatus; }
	inline QUrl logoUrl() const { return mLogoUrl; }
	inline int numViews() const { return mNumViews; }
	inline int numFollowers() const { return mNumFollowers; }
	inline bool isMature() const { return mMature; }
	inline bool isPartnered() const { return mPartnered; }
	inline bool isFollowed() const { return mFollowed; }
	inline QString category() const { return mCategory; }
	inline bool isOnline() const { return !mCategory.isEmpty(); }

	virtual QString logoCacheString() const;
	static QString logoCacheString(QString pTag);

	virtual ChannelChat * chat() = 0;

	void clear();
	bool updateFrom(ChannelObject const & pOther);

signals:
	void statsChanged();

protected:
	void setStats(int pFollowers, int pViews);

	QVariant mId;
	QString  mName;
	QString  mDisplayName;
	QString  mStatus;
	QUrl     mLogoUrl;
	int      mNumViews;
	int      mNumFollowers;
	bool     mPartnered;
	bool     mMature;
	bool     mFollowed;
	QString  mCategory;
};

#endif // CORE_CHANNEL_OBJECT_H_
