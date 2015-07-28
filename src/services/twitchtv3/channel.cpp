#include "channel.h"

#include <QByteArray>
#include <QJsonDocument>
#include <QMap>
#include <QMetaType>
#include <QNetworkRequest>
#include <QUrl>
#include "channel_chat.h"
#include "profile.h"
#include "server_reply.h"
#include "user.h"

#include "../../core/class_bitset.h"
#include "../../core/i_user.h"
#include "../../core/reply_base.h"
#include "../../misc.h"

namespace twitchtv3
{

Channel::Channel(User & pUser) : IChannel(pUser)
{
	mChat = new ChannelChat(*this, pUser.profile().chatServer());

	if (mUser.isSelf())
	{
		mFlags.set(Flag::FOLLOWED);
		mFlags.set(Flag::AM_EDITOR);
	}
}

Channel::~Channel()
{
}

Profile & Channel::profile() const
{
	return owner().profile();
}

User & Channel::owner() const
{
	return static_cast<User&>(mUser);
}

QString Channel::name() const
{
	return owner().name();
}

QString Channel::logoCacheString() const
{
	return IChannel::logoCacheString(QString("twitchtv3:%1").arg(name()));
}

IChannelChat * Channel::chat()
{
	return mChat;
}

void Channel::refresh()
{
	QNetworkRequest lRequest = profile().serviceRequest(true);
	lRequest.setUrl(profile().krakenUrl(QString("/streams/%1").arg(name())));

	profile().throttledGet(lRequest, [this] (QNetworkReply & pReply)
	{
		ServerReply lReply(profile(), pReply, "StreamInfo");
		if (lReply.hasError())
			return;

		updateFromVariant(lReply.data().value("stream"));

		if (mNumViews == -1)
		{
			QNetworkRequest lRequest = profile().serviceRequest(true);
			lRequest.setUrl(profile().krakenUrl(QString("/channels/%1").arg(name())));

			profile().throttledGet(lRequest, [this] (QNetworkReply & pReply)
			{
				ServerReply lReply(profile(), pReply, "ChannelInfo");
				if (lReply.hasError())
					return;

				updateFromVariant(lReply.data());
			});
		}
	});
}

void Channel::modifyStreamSettings(QString pTitle, ICategory * pCategory, bool pMature, int pDelay)
{
	QNetworkRequest lRequest = profile().serviceRequest(true);
	lRequest.setUrl(profile().krakenUrl(QString("/channels/%1/").arg(name())));
	lRequest.setHeader(lRequest.ContentTypeHeader, "application/json");

	QVariantMap lChannel;
	lChannel["status"] = pTitle;
	lChannel["game"] = (pCategory ? pCategory->name() : QString());
	if (isPartnered() && amEditor())
		lChannel["delay"] = pDelay;

	QVariantMap lData;
	lData["channel"] = lChannel;

	QJsonDocument lJson = QJsonDocument::fromVariant(lData);

	profile().throttledPut(lRequest, lJson.toJson(QJsonDocument::Compact), [this] (QNetworkReply & pReply)
	{
		ServerReply lReply(profile(), pReply, "UpdateChannel");
		if (lReply.hasError())
			return;

		updateFromVariant(lReply.data());
	});
}

QUrl Channel::getStreamUrl(UrlType pType)
{
	switch (pType)
	{
		case URL_CHANNEL:
		case URL_STREAM_WEBSITE:
			return QUrl(QString("http://www.twitch.tv/%1").arg(name()));

		case URL_STREAM_DIRECT:
			break;
	}

	return QUrl();
}

void Channel::updateFromVariant(QVariant const & pValue)
{
	QVariantMap lItem = pValue.toMap();
	if (lItem.isEmpty())
		return;

	QVariantMap lChannel;
	if (lItem.contains("channel"))
		lChannel = lItem.value("channel").toMap();
	else
		lChannel = lItem;

	bool lOk;
	bool lChanged = false;
	lChanged |= updateIfChanged(mDisplayName, lChannel.value("display_name").toString());
	lChanged |= updateIfChanged(mTitle, lChannel.value("status").toString());
	lChanged |= updateIfChanged(mLogoUrl, lChannel.value("logo").toUrl());
	lChanged |= updateIfChanged(mNumFollowers, lChannel.value("followers").toInt(&lOk), lOk);
	lChanged |= updateIfChanged(mNumViews, lChannel.value("views").toInt(&lOk), lOk);
	lChanged |= updateIfChanged(mDelay, lChannel.value("delay", -1).toInt(&lOk), lOk);
	lChanged |= updateIfChanged<Flag>(mFlags, Flag::MATURE, lChannel.value("mature").toBool());
	lChanged |= updateIfChanged<Flag>(mFlags, Flag::PARTNERED, lChannel.value("partner").toBool());

	QString lCategory = lChannel.value("game").toString();
	lChanged |= updateIfChanged(mCategory, profile().getCategoryFor(lCategory));

	if (lItem.contains("viewers"))
	{
		lChanged |= updateIfChanged(mOnlineSince, lItem.value("created_at").toDateTime());
		lChanged |= updateIfChanged(mNumViewers, lItem.value("viewers").toInt(&lOk), lOk);
		//mFPS = lItem.value("average_fps").toDouble();
	}
	else
	{
		lChanged |= updateIfChanged(mOnlineSince, QDateTime());
		lChanged |= updateIfChanged(mNumViewers, -1);

		if (lItem.contains("notifications"))
		{
			lChanged |= updateIfChanged(mFollowedSince, lItem.value("created_at").toDateTime());
			lChanged |= updateIfChanged<Flag>(mFlags, Flag::FOLLOWED, true);
		}
	}

	if (lChanged)
		emit infoUpdated();
}

} // namespace twitchtv3
