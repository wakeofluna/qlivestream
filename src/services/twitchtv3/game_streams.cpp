#include "config.h"
#include "game_streams.h"
#include "channel.h"
#include "profile.h"
#include <QList>

namespace twitchtv3
{

GameStreams::GameStreams(Profile & pProfile, QNetworkReply & pReply) : ServerReply(pProfile, pReply)
{
	if (!parse())
		return;
}

GameStreams::~GameStreams()
{

}

QString GameStreams::tag() const
{
	return QStringLiteral("GameStreams");
}

QList<ChannelObject*> GameStreams::createList() const
{
	QVariantList lList = mData.value("streams").toList();

	QList<ChannelObject*> lData;
	lData.reserve(lList.size());

	for (int i = 0; i < lList.size(); ++i)
		lData.push_back(new Channel(*profile(), lList[i], true));

	return lData;
}

} // namespace twitchtv3
