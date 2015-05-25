#ifndef SERVICES_TWITCHTV3_GAMES_TOP_H_
#define SERVICES_TWITCHTV3_GAMES_TOP_H_

#include "server_reply.h"

class CategoryObject;
template <typename T> class QList;

namespace twitchtv3
{

class GamesTop : public ServerReply
{
public:
	GamesTop(QNetworkReply & pReply);
	~GamesTop();

	QString tag() const override;

	QList<CategoryObject*> createList() const;
};

} // namespace twitchtv3

#endif // SERVICES_TWITCHTV3_GAMES_TOP_H_
