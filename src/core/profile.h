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

	inline QString name() const { return mName; }
	inline QString account() const { return mAccount; }
	inline QString token() const { return mAuthToken; }
	inline AuthScopes const & requested() const { return mRequested; }
	inline AuthScopes const & privileges() const { return mAuthScope; }

private:
	friend class forms::SelectProfile;
	friend class forms::EditProfile;
	friend class forms::PerformLogin;
	Profile();

	static QStringList listProfiles();
	static Profile load(QString pName);
	static void erase(QString pName);
	void save() const;
	void saveAndReplace(QString pOldName) const;

	static void eraseImpl(QSettings & pSettings, QString pName);
	void saveImpl(QSettings & pSettings) const;

	QString    mName;
	QString    mAccount;
	QString    mAuthToken;
	AuthScopes mRequested;
	AuthScopes mAuthScope;
};

#endif // CORE_PROFILE_H_
