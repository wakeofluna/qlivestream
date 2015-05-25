#include "config.h"
#include "channels_object.h"

#include <QVariant>
#include <QVariantMap>

namespace twitchtv3
{

ChannelsObject::ChannelsObject(QVariant const & pValue, bool pFollowing)
{
	QVariantMap lItem = pValue.toMap();

	QVariantMap lChannel;
	if (lItem.contains("channel"))
		lChannel = lItem.value("channel").toMap();
	else
		lChannel = lItem;

	mName = lChannel.value("name").toString();
	mDisplayName = lChannel.value("display_name").toString();
	mStatus = lChannel.value("status").toString();
	mLogoUrl = lChannel.value("logo").toString();
	mCategory = lChannel.value("game").toString();
	mMature = lChannel.value("mature").toBool();
	mNumFollowers = lChannel.value("followers").toInt();
	mNumViews = lChannel.value("views").toInt();
	mPartnered = lChannel.value("partner").toBool();
	mFollowed = pFollowing;
}

ChannelsObject::~ChannelsObject()
{

}

QString ChannelsObject::logoCacheString() const
{
	return ChannelObject::logoCacheString(QString("twitchtv3:%1").arg(mName));
}

} // namespace twitchtv3
