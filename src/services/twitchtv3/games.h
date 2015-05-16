#ifndef SERVICES_TWITCHTV3_GAMES_H_
#define SERVICES_TWITCHTV3_GAMES_H_

#include "server_reply.h"

#include <QVector>
class CategoryObject;

namespace twitchtv3
{

class Games : public ServerReply
{
public:
	Games(QNetworkReply & pReply);
	~Games();

	QString tag() const override;

	QVector<CategoryObject*> createList() const;
};

} // namespace twitchtv3

#endif // SERVICES_TWITCHTV3_GAMES_H_
