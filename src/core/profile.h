#ifndef CORE_PROFILE_H_
#define CORE_PROFILE_H_

#include "auth_scope.h"

#include <QString>
class QStringList;

namespace forms
{
	class SelectProfile;
	class EditProfile;
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
	Profile();

	static QStringList listProfiles();
	static Profile load(QString pName);
	void save() const;

	QString    mName;
	QString    mAccount;
	AuthScopes mRequested;
	QString    mAuthToken;
	AuthScopes mAuthScope;
};

#endif // CORE_PROFILE_H_
