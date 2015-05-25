#ifndef SERVICES_TWITCHTV3_PROFILE_H_
#define SERVICES_TWITCHTV3_PROFILE_H_

#include "core/profile.h"
#include "auth_scope.h"

#include <QList>
class CategoryObject;
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
	void getFollowedCategories(int pStart, int pLimit, CategoryCallback && pCallback) override;
	void getTopCategories(int pStart, int pLimit, CategoryCallback && pCallback) override;
	void getFollowedChannels(int pStart, int pLimit, ChannelCallback && pCallback) override;
	void getCategoryChannels(CategoryObject * pCategory, int pStart, int pLimit, ChannelCallback && pCallback) override;

private:
	QUrl apiUrl(QString pAppend) const;
	QUrl krakenUrl(QString pAppend = QString()) const;
	QNetworkRequest serviceRequest(bool pAuthed = true) const;

	AuthScopes mScopes;
};

} // namespace twitchtv3

#endif // SERVICES_TWITCHTV3_PROFILE_H_
