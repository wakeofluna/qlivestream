#include "config.h"
#include "followed_games.h"
#include "games_object.h"

#include <QList>
#include <QString>

namespace twitchtv3
{

FollowedGames::FollowedGames(QNetworkReply & pReply) : ServerReply(pReply)
{
	if (!parse())
		return;
}

FollowedGames::~FollowedGames()
{

}

QString FollowedGames::tag() const
{
	return QStringLiteral("FollowedGames");
}

QList<CategoryObject*> FollowedGames::createList() const
{
	QVariantList lList = mData.value("follows").toList();

	QList<CategoryObject*> lData;
	lData.reserve(lList.size());

	for (int i = 0; i < lList.size(); ++i)
		lData.push_back(new GamesObject(lList[i], true));

	return lData;
}

} // namespace twitchtv3
