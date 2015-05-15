#ifndef CORE_PROFILE_H_
#define CORE_PROFILE_H_

#include <memory>
#include <QString>

namespace forms
{
	class PerformLogin;
}

class Profile
{
public:
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

	virtual QString service() const = 0;

protected:
	friend class forms::PerformLogin;
	Profile();

	void updateToken(QString pToken) const;

	int        mId;
	QString    mAccount;
	QString    mToken;
	Level      mLevel;
};

#endif // CORE_PROFILE_H_
