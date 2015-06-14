#include "channel.h"

#include "config.h"
#include <QVariant>
#include <QVariantMap>

namespace twitchtv3
{

Channel::Channel(QString pName)
{
	mName = pName;
}

Channel::Channel(QVariant const & pValue, bool pFollowing)
{
	updateFromVariant(pValue);
	mFollowed = pFollowing;
}

Channel::~Channel()
{

}

QString Channel::logoCacheString() const
{
	return ChannelObject::logoCacheString(QString("twitchtv3:%1").arg(mName));
}

ChannelChat * Channel::chat()
{
	return nullptr;
}

void Channel::updateFromVariant(QVariant const & pValue)
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
}

} // namespace twitchtv3
