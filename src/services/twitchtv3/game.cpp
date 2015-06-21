#include "config.h"
#include "game.h"
#include "profile.h"

#include <QVariant>
#include <QVariantMap>

namespace twitchtv3
{

Game::Game(Profile & pProfile, QVariant const & pValue, bool pFollowing) : CategoryObject(pProfile)
{
	QVariantMap lItem = pValue.toMap();

	QVariantMap lGame;
	if (lItem.contains("game"))
	{
		lGame = lItem.value("game").toMap();
		mNumChannels = lItem.value("channels", -1).toInt();
		mNumViewers = lItem.value("viewers", -1).toInt();
	}
	else
		lGame = lItem;

	QVariantMap lBox = lGame.value("box").toMap();

	mId = lGame.value("_id").toInt();
	mName = lGame.value("name").toString();
	mGiantBombId = lGame.value("giantbomb_id").toInt();
	mLogoUrl = lBox.value("medium").toString();
	mFollowed = pFollowing;
}

Game::~Game()
{
}

Profile * Game::profile() const
{
	return static_cast<Profile*>(&mProfile);
}

QString Game::logoCacheString() const
{
	return CategoryObject::logoCacheString(QString("twitchtv3:%1").arg(mName));
}

} // namespace twitchtv3
