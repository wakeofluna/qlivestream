#ifndef CORE_PROFILE_H_
#define CORE_PROFILE_H_

#include <functional>
#include <memory>
#include <QString>

class ConfigProfile;
class QUrl;

namespace forms
{
	class PerformLogin;
}

class Profile
{
public:
	typedef std::function<void ()> DefaultCallback;
	typedef std::unique_ptr<Profile> Ptr;

public:
	enum Level
	{
		CLIENT = 0,
		MODERATOR = 1,
		STREAMER = 2
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
