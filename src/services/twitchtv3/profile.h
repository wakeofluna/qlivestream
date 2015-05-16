#ifndef SERVICES_TWITCHTV3_PROFILE_H_
#define SERVICES_TWITCHTV3_PROFILE_H_

#include "core/profile.h"
#include "auth_scope.h"

class QUrl;
class QNetworkRequest;

namespace twitchtv3
{

class Profile : public ::Profile
{
public:
	Profile();
	~Profile();

	QUrl acquireTokenUrl() const override;
	void performLogin(DefaultCallback && pCallback) override;
	void getTopCategories(int pStart, int pLimit, CategoryCallback && pCallback) override;
	void getFollowings(int pStart, int pLimit, ChannelCallback && pCallback) override;

private:
	QUrl serviceUrl(QString pAppend = QString()) const;
	QNetworkRequest serviceRequest() const;

	AuthScopes mScopes;
};

} // namespace twitchtv3

#endif // SERVICES_TWITCHTV3_PROFILE_H_
