#include "config.h"
#include "channel.h"
#include "chat_channel.h"
#include "chat_server.h"
#include "profile.h"
#include "server_reply.h"

#include <QDateTime>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QUrl>
#include <QVariant>
#include <QVariantMap>

namespace twitchtv3
{

Channel::Channel(Profile & pProfile, QString pName) : ChannelObject(pProfile)
{
	mName = pName;
	mChat = new ChatChannel(*this, pProfile.chatServer());
}

Channel::Channel(Profile & pProfile, QVariant const & pValue, bool pFollowing) : ChannelObject(pProfile)
{
	updateFromVariant(pValue);
	mChat = new ChatChannel(*this, pProfile.chatServer());
	mFollowed = pFollowing;
}

Channel::~Channel()
{
	delete mChat;
}

Profile * Channel::profile() const
{
	return static_cast<Profile*>(&mProfile);
}

QString Channel::logoCacheString() const
{
	return ChannelObject::logoCacheString(QString("twitchtv3:%1").arg(mName));
}

ChannelChat * Channel::chat()
{
	return mChat;
}

void Channel::requestUpdate()
{
	QNetworkRequest lRequest = profile()->serviceRequest(true);
	lRequest.setUrl(profile()->krakenUrl(QString("/channels/%1").arg(mName)));

	profile()->throttledGet(lRequest, [this] (QNetworkReply & pReply)
	{
		ServerReplySimple lReply(*profile(), pReply, "GetChannel");
		if (!lReply.hasError())
			updateFromVariant(lReply.data());
	});
}

void Channel::updateStreamSettings(QString pTitle, QString pCategory, bool pMature, int pDelay)
{
	QNetworkRequest lRequest = profile()->serviceRequest(true);
	lRequest.setUrl(profile()->krakenUrl(QString("/channels/%1/").arg(mName)));
	lRequest.setHeader(lRequest.ContentTypeHeader, "application/json");

	QVariantMap lChannel;
	lChannel["status"] = pTitle;
	lChannel["game"] = pCategory;
	if (mPartnered && mSelf)
		lChannel["delay"] = pDelay;

	QVariantMap lData;
	lData["channel"] = lChannel;

	QJsonDocument lJson = QJsonDocument::fromVariant(lData);

	profile()->throttledPut(lRequest, lJson.toJson(QJsonDocument::Compact), [this] (QNetworkReply & pReply)
	{
		ServerReplySimple lReply(*profile(), pReply, "UpdateChannel");
		if (!lReply.hasError())
			updateFromVariant(lReply.data());
	});
}

QUrl Channel::getStreamUrl(UrlType pType)
{
	switch (pType)
	{
		case URL_CHANNEL:
		case URL_STREAM_WEBSITE:
			return QUrl(QString("http://www.twitch.tv/%1").arg(mName));

		case URL_STREAM_DIRECT:
			break;
	}

	return QUrl();
}

void Channel::updateFromVariant(QVariant const & pValue)
{
	QVariantMap lItem = pValue.toMap();

	QVariantMap lChannel;
	if (lItem.contains("channel"))
		lChannel = lItem.value("channel").toMap();
	else
		lChannel = lItem;

	bool lOk;
	mName = lChannel.value("name").toString();
	mDisplayName = lChannel.value("display_name").toString();
	mStatus = lChannel.value("status").toString();
	mLogoUrl = lChannel.value("logo").toString();
	mCategory = lChannel.value("game").toString();
	mMature = lChannel.value("mature").toBool();
	mNumFollowers = lChannel.value("followers").toInt();
	mNumViews = lChannel.value("views").toInt();
	mPartnered = lChannel.value("partner").toBool();

	mDelay = lChannel.value("delay").toInt(&lOk);
	if (!lOk)
		mDelay = -1;

	if (mDisplayName.isEmpty())
		mDisplayName = mName;

	bool lSelf = mName == mProfile.account();
	mSelf = lSelf;
	mEditor = lSelf;

	if (lItem.contains("viewers"))
	{
		mOnlineSince = lItem.value("created_at").toDateTime();
		mNumViewers = lItem.value("viewers").toInt();
		mFPS = lItem.value("average_fps").toDouble();
	}
	else
	{
		mOnlineSince = QDateTime();
		mNumViewers = -1;
	}

	emit statsChanged();
}

} // namespace twitchtv3
