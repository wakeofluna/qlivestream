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

	inline AuthScopes const & privileges() const { return mAuthScope; }

private:
	friend class forms::SelectProfile;
	friend class forms::EditProfile;
	friend class forms::PerformLogin;
	Profile();

	static QStringList listProfiles();
	static Profile load(QString pName);
	static void erase(QString pName);
	void saveAndReplace(QString pOldName) const;

	static void eraseImpl(QSettings & pSettings, QString pName);
	void saveImpl(QSettings & pSettings) const;

	QString    mName;
	QString    mAccount;
	AuthScopes mRequested;
	QString    mAuthToken;
	AuthScopes mAuthScope;
};

#endif // CORE_PROFILE_H_
