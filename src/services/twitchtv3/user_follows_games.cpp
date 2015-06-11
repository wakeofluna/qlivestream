#include "config.h"
#include "user_follows_games.h"
#include <QList>
#include "game.h"

namespace twitchtv3
{

UserFollowsGames::UserFollowsGames(QNetworkReply & pReply) : ServerReply(pReply)
{
	if (!parse())
		return;
}

UserFollowsGames::~UserFollowsGames()
{

}

QString UserFollowsGames::tag() const
{
	return QStringLiteral("UserFollowsGames");
}

QList<CategoryObject*> UserFollowsGames::createList() const
{
	QVariantList lList = mData.value("follows").toList();

	QList<CategoryObject*> lData;
	lData.reserve(lList.size());

	for (int i = 0; i < lList.size(); ++i)
		lData.push_back(new Game(lList[i], true));

	return lData;
}

} // namespace twitchtv3
