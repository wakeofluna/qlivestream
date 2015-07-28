#ifndef CORE_I_PROFILE_H_
#define CORE_I_PROFILE_H_

#include <functional>
#include <QByteArray>
#include <QList>
#include <QString>
#include <QUrl>
#include "i_category.h"
#include "i_channel.h"
#include "network_access.h"

#include "../config.h"

namespace forms
{
	class PerformLogin;
}

class COREDLL IProfile : public QObject, protected NetworkAccess
{
Q_OBJECT

public:
	typedef std::function<void ()> DefaultCallback;
	typedef std::function<void (QByteArray &&)> DataCallback;
	typedef std::function<void (QList<ICategory*> &&)> CategoryCallback;
	typedef std::function<void (QList<IChannel*> &&)> ChannelCallback;

public:
	enum Level
	{
		ANONYMOUS = 0,
		VIEWER = 1,
		EDITOR = 2,
		STREAMER = 3
	};

public:
	virtual ~IProfile();

	inline int id() const { return mId; }
	inline QString account() const { return mAccount; }
	inline QString token() const { return mToken; }
	inline Level level() const { return mLevel; }
	inline bool loggedIn() const { return mLoggedIn; }

	inline bool hasError() const { return !mLastError.isEmpty(); }
	inline QString lastError() const { return mLastError; }
	void setLastError(QString pError);

	virtual QUrl acquireTokenUrl() const = 0;
	virtual void performLogin(DefaultCallback && pCallback) = 0;
	virtual void downloadLogo(QUrl const & pUrl, DataCallback && pCallback);

	inline QList<IChannel *> followedChannels() const { return mFollowedChannels; }
	inline QList<ICategory *> followedCategories() const { return mFollowedCategories; }
	inline QList<ICategory *> topCategories() const { return mTopCategories; }

	inline IUser & self() const { return *mSelf; }
	IUser * getUserFor(QString pName, bool pCreate = true);
	ICategory * getCategoryFor(QString pName, bool pCreate = true);

public slots:
	void resetFollowedChannels();
	void resetFollowedCategories();
	void resetTopCategories();
	virtual void rollupFollowedChannels() = 0;
	virtual void rollupFollowedCategories() = 0;
	virtual void rollupTopCategories() = 0;

signals:
	void followedChannelsUpdated();
	void followedCategoriesUpdated();
	void topCategoriesUpdated();

protected:
	friend class ConfigProfile;
	friend class forms::PerformLogin;
	IProfile();

	virtual void initProfile();

	void updateToken(QString pToken);

	virtual IUser * newUserFor(QString pName) = 0;
	virtual ICategory * newCategoryFor(QString pName) = 0;

	int        mId;
	QString    mAccount;
	QString    mToken;
	Level      mLevel;
	bool       mLoggedIn;
	QString    mLastError;

	IUser * mSelf;
	QHash<QString, IUser *> mUsers;
	QHash<QString, ICategory *> mCategories;

	bool mCanRollupFollowedChannels;
	bool mCanRollupFollowedCategories;
	bool mCanRollupTopCategories;
	QList<IChannel *>  mFollowedChannels;
	QList<ICategory *> mFollowedCategories;
	QList<ICategory *> mTopCategories;
};

#endif // CORE_I_PROFILE_H_
