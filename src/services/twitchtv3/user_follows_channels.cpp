#include "config.h"
#include "user_follows_channels.h"
#include "channels_object.h"

#include <QList>

namespace twitchtv3
{

UserFollowsChannels::UserFollowsChannels(QNetworkReply & pReply) : ServerReply(pReply)
{
	if (!parse())
		return;
}

UserFollowsChannels::~UserFollowsChannels()
{

}

QString UserFollowsChannels::tag() const
{
	return QStringLiteral("UserFollowsChannels");
}

QList<ChannelObject*> UserFollowsChannels::createList() const
{
	QVariantList lList = mData.value("follows").toList();

	QList<ChannelObject*> lData;
	lData.reserve(lList.size());

	for (int i = 0; i < lList.size(); ++i)
		lData.push_back(new ChannelsObject(lList[i]));

	return lData;
}

} // namespace twitchtv3
