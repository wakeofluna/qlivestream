#ifndef CORE_PROFILE_H_
#define CORE_PROFILE_H_

#include "auth_scope.h"

#include <QString>
class QSettings;
class QStringList;

namespace forms
{
	class SelectProfile;
	class EditProfile;
	class PerformLogin;
}

class Profile
{
public:
	~Profile();

	bool isValid() const;

	inline QString account() const { return mAccount; }
	inline QString token() const { return mToken; }
	inline AuthScopes const & requested() const { return mRequested; }
	inline AuthScopes const & privileges() const { return mAuthScope; }

private:
	friend class forms::SelectProfile;
	friend class forms::EditProfile;
	friend class forms::PerformLogin;
	Profile();

	static QStringList listProfiles();
	static Profile load(QString pName);
	void erase() const;
	void save() const;

	int        mId;
	QString    mAccount;
	QString    mToken;
	AuthScopes mRequested;
	AuthScopes mAuthScope;
};

#endif // CORE_PROFILE_H_
