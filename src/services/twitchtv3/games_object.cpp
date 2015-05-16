#include "config.h"
#include "games_object.h"

#include <QCryptographicHash>
#include <QVariant>
#include <QVariantMap>

namespace twitchtv3
{

GamesObject::GamesObject(QVariant const & pValue)
{
	QVariantMap lItem = pValue.toMap();
	QVariantMap lGame = lItem.value("game").toMap();
	QVariantMap lBox = lGame.value("box").toMap();

	mName = lGame.value("name").toString();
	mId = lGame.value("_id").toInt();
	mGiantBombId = lGame.value("giantbomb_id").toInt();
	mLogoUrl = lBox.value("medium").toString();

	updateFrom(pValue);
}

GamesObject::~GamesObject()
{
}

void GamesObject::updateFrom(QVariant const & pValue)
{
	QVariantMap lItem = pValue.toMap();

	int lChannels = lItem.value("channels").toInt();
	int lViewers = lItem.value("viewers").toInt();

	setStats(lChannels, lViewers);
}

QString GamesObject::logoCacheString() const
{
	return QCryptographicHash::hash(QString("twitchtv3:%1").arg(mName).toUtf8(), QCryptographicHash::Md5).toHex();
}

} // namespace twitchtv3
