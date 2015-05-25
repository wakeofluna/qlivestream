#include "config.h"
#include "games_object.h"

#include <QCryptographicHash>
#include <QVariant>
#include <QVariantMap>

namespace twitchtv3
{

GamesObject::GamesObject(QVariant const & pValue, bool pFollowing)
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

GamesObject::~GamesObject()
{
}

QString GamesObject::logoCacheString() const
{
	return QCryptographicHash::hash(QString("twitchtv3:%1").arg(mName).toUtf8(), QCryptographicHash::Md5).toHex();
}

} // namespace twitchtv3
