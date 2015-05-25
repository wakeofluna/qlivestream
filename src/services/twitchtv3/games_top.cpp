#include "config.h"
#include "games_top.h"
#include "games_object.h"

#include <QList>

namespace twitchtv3
{

GamesTop::GamesTop(QNetworkReply & pReply) : ServerReply(pReply)
{
	if (!parse())
		return;
}

GamesTop::~GamesTop()
{

}

QString GamesTop::tag() const
{
	return QStringLiteral("TopGames");
}

QList<CategoryObject*> GamesTop::createList() const
{
	QVariantList lList = mData.value("top").toList();

	QList<CategoryObject*> lData;
	lData.reserve(lList.size());

	for (int i = 0; i < lList.size(); ++i)
		lData.push_back(new GamesObject(lList[i]));

	return lData;
}

} // namespace twitchtv3
