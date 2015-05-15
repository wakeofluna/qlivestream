#ifndef SERVICES_TWITCHTV3_PROFILE_H_
#define SERVICES_TWITCHTV3_PROFILE_H_

#include "core/network_access.h"
#include "core/profile.h"
#include "auth_scope.h"

class QUrl;
class QNetworkRequest;

namespace twitchtv3
{

class Profile : public ::Profile, public NetworkAccess
{
public:
	Profile();
	~Profile();

	QUrl acquireTokenUrl() const;
	void performLogin(DefaultCallback && pCallback);

private:
	QUrl serviceUrl() const;
	QNetworkRequest serviceRequest() const;

	AuthScopes mScopes;
};

} // namespace twitchtv3

#endif // SERVICES_TWITCHTV3_PROFILE_H_
