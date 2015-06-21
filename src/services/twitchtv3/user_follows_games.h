#ifndef SERVICES_TWITCHTV3_USER_FOLLOWS_GAMES_H_
#define SERVICES_TWITCHTV3_USER_FOLLOWS_GAMES_H_

#include "server_reply.h"

class CategoryObject;
template <typename T> class QList;

namespace twitchtv3
{

class Profile;
class UserFollowsGames : public ServerReply
{
public:
	UserFollowsGames(Profile & pProfile, QNetworkReply & pReply);
	~UserFollowsGames();

	QString tag() const override;

	QList<CategoryObject*> createList() const;
};

} // namespace twitchtv3

#endif // SERVICES_TWITCHTV3_USER_FOLLOWS_GAMES_H_
