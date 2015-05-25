#ifndef SERVICES_TWITCHTV3_FOLLOWED_GAMES_H_
#define SERVICES_TWITCHTV3_FOLLOWED_GAMES_H_

#include "server_reply.h"

class CategoryObject;
template <typename T> class QList;

namespace twitchtv3
{

class FollowedGames : public ServerReply
{
public:
	FollowedGames(QNetworkReply & pReply);
	~FollowedGames();

	QString tag() const override;

	QList<CategoryObject*> createList() const;
};

} // namespace twitchtv3

#endif // SERVICES_TWITCHTV3_FOLLOWED_GAMES_H_
