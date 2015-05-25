#include "config.h"
#include "games.h"
#include "games_object.h"

#include <QList>

namespace twitchtv3
{

Games::Games(QNetworkReply & pReply) : ServerReply(pReply)
{
	if (!parse())
		return;
}

Games::~Games()
{

}

QString Games::tag() const
{
	return QStringLiteral("TopGames");
}

QList<CategoryObject*> Games::createList() const
{
	QVariantList lList = mData.value("top").toList();

	QList<CategoryObject*> lData;
	lData.reserve(lList.size());

	for (int i = 0; i < lList.size(); ++i)
		lData.push_back(new GamesObject(lList[i]));

	return lData;
}

} // namespace twitchtv3
