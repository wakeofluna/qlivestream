#ifndef CORE_PROFILE_H_
#define CORE_PROFILE_H_

#include "core/network_access.h"
#include <functional>
#include <memory>
#include <QString>
#include <QVector>

class CategoryObject;
class ChannelObject;
class ConfigProfile;
class QByteArray;
class QUrl;

namespace forms
{
	class PerformLogin;
}

class Profile : public NetworkAccess
{
public:
	typedef std::function<void ()> DefaultCallback;
	typedef std::function<void (QByteArray const&)> DataCallback;
	typedef std::function<void (QVector<CategoryObject*> const&)> CategoryCallback;
	typedef std::function<void (QVector<ChannelObject*> const&)> ChannelCallback;
	typedef std::unique_ptr<Profile> UPtr;

public:
	enum Level
	{
		ANONYMOUS = 0,
		VIEWER = 1,
		MODERATOR = 2,
		STREAMER = 3
	};

public:
	virtual ~Profile();

	inline int id() const { return mId; }
	inline QString account() const { return mAccount; }
	inline QString token() const { return mToken; }
	inline Level level() const { return mLevel; }
	inline bool loggedIn() const { return mLoggedIn; }

	inline bool hasError() const { return !mLastError.isEmpty(); }
	inline QString lastError() const { return mLastError; }

	virtual QUrl acquireTokenUrl() const = 0;
	virtual void performLogin(DefaultCallback && pCallback) = 0;
	virtual void getTopCategories(int pStart, int pLimit, CategoryCallback && pCallback) = 0;
	virtual void getFollowings(int pStart, int pLimit, ChannelCallback && pCallback) = 0;
	virtual void downloadLogo(QUrl const & pUrl, DataCallback && pCallback);

protected:
	friend class ConfigProfile;
	friend class forms::PerformLogin;
	Profile();

	void updateToken(QString pToken);

	int        mId;
	QString    mAccount;
	QString    mToken;
	Level      mLevel;
	bool       mLoggedIn;
	QString    mLastError;
};

#endif // CORE_PROFILE_H_
