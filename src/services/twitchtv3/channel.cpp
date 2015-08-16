#include "channel.h"

#include <QByteArray>
#include <QJsonDocument>
#include <QMap>
#include <QMetaType>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>
#include "channel_chat.h"
#include "profile.h"
#include "server_reply.h"
#include "user.h"
#include "video.h"

#include "../../core/class_bitset.h"
#include "../../core/i_user.h"
#include "../../core/i_video.h"
#include "../../core/reply_base.h"
#include "../../core/reply_text.h"
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

QUrl Channel::streamUrl(UrlType pType)
{
	switch (pType)
	{
		case URL_LOGO:
			return mLogoUrl;

		case URL_CHANNEL:
		case URL_STREAM_WEBSITE:
			return QUrl(QString("http://www.twitch.tv/%1").arg(name()));

		case URL_STREAM_DIRECT:
		{
			QNetworkRequest lRequest;
			QUrl lUrl;
			QUrlQuery lParams;

			lUrl = profile().apiUrl(QString("/channels/%1/access_token").arg(name()));
			lRequest = profile().serviceRequest(true);
			lRequest.setUrl(lUrl);

			ServerReply lReply(profile(), *profile().synchronisedGet(lRequest), "ChannelToken");
			if (lReply.hasError())
				break;

			lParams.clear();
			lParams.addQueryItem("p", QString::number(int(double(qrand()) / RAND_MAX * 999999)));
			lParams.addQueryItem("type", "any");
			lParams.addQueryItem("allow_source", "true");
			lParams.addQueryItem("allow_audio_only", "true");
			lParams.addQueryItem("sig", lReply.data().value("sig").toString());
			lParams.addQueryItem("token", lReply.data().value("token").toString());
			lUrl = profile().usherUrl(QString("/api/channel/hls/%1.m3u8").arg(name()));
			lUrl.setQuery(lParams);
			lRequest = profile().serviceRequest(false);
			lRequest.setUrl(lUrl);

			ReplyText lReply2(profile(), *profile().synchronisedGet(lRequest), "ChannelPlaylist");
			if (lReply2.hasError())
				break;

			QStringList lPlaylist = lReply2.data();
			if (lPlaylist.isEmpty() || lPlaylist[0] != "#EXTM3U")
			{
				qCritical() << "Received playlist is not a valid M3U playlist?";
				break;
			}

			// Grab the very first valid line, should be best available quality
			for (QString & lLine : lPlaylist)
			{
				if (lLine[0] == '#')
					continue;

				QUrl lUrl(lLine);
				if (lUrl.isValid())
					return lUrl;
			}

			break;
		}
	}

	return QUrl();
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

		if (lReply.data().contains("stream"))
		{
			QVariant lStreamData = lReply.data().value("stream");
			if (lStreamData.isNull())
			{
				bool lChanged = false;
				lChanged |= updateIfChanged(mOnlineSince, QDateTime());
				lChanged |= updateIfChanged(mNumViewers, -1);
				if (lChanged)
					emit infoUpdated();
			}
			else
			{
				updateFromVariant(lStreamData);
			}
		}

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

void Channel::setFollowed(bool pFollow)
{
	if (profile().hasScope(AuthScope::user_follows_edit))
	{
		QNetworkRequest lRequest = profile().serviceRequest(true);
		lRequest.setUrl(profile().krakenUrl(QString("/users/%1/follows/channels/%2").arg(profile().account()).arg(name())));

		if (pFollow)
		{
			profile().throttledPut(lRequest, QByteArray(), [this] (QNetworkReply & pReply)
			{
				ServerReply lReply(profile(), pReply, "FollowChannel");
				if (lReply.hasError())
					return;

				updateFromVariant(lReply.data());
			});
		}
		else
		{
			profile().throttledDelete(lRequest, [this] (QNetworkReply & pReply)
			{
				ReplyBase lReply(profile(), pReply, "UnfollowChannel");

				bool lOk = lReply.checkNetworkStatus();
				lReply.log();

				if (lOk)
				{
					if (updateIfChanged<Flag>(mFlags, Flag::FOLLOWED, false))
						emit infoUpdated();
				}
			});
		}
	}
}

void Channel::rollupVideos()
{
	if (!mCanRollupVideos)
		return;

	mCanRollupVideos = false;

	QUrlQuery lUrlQuery;
	lUrlQuery.addQueryItem("game", name());
	lUrlQuery.addQueryItem("limit", "5");
	lUrlQuery.addQueryItem("offset", QString::number(mVideos.count()));

	QUrl lUrl = profile().krakenUrl(QString("/channels/%1/videos").arg(name()));
	lUrl.setQuery(lUrlQuery);

	QNetworkRequest lRequest = profile().serviceRequest(false);
	lRequest.setUrl(lUrl);

	Logger::StatusMessage lStatus("Getting videos for channel...");

	profile().throttledGet(lRequest, [this, lStatus] (QNetworkReply & pReply)
	{
		twitchtv3::ServerReply lReply(profile(), pReply, "ChannelVideos");
		if (lReply.hasError())
			return;

		QVariantList lList = lReply.data().value("videos").toList();
		if (!lList.isEmpty())
		{
			mCanRollupVideos = (lList.size() == 5);

			for (QVariant & lVideoItem : lList)
				processVideoObject(lVideoItem);

			std::sort(mVideos.begin(), mVideos.end());
			emit videosUpdated();
		}
	});
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
	else if (lItem.contains("notifications"))
	{
		lChanged |= updateIfChanged(mFollowedSince, lItem.value("created_at").toDateTime());
		lChanged |= updateIfChanged<Flag>(mFlags, Flag::FOLLOWED, true);
	}

	if (lChanged)
		emit infoUpdated();
}

IVideo * Channel::processVideoObject(QVariant pVideo)
{
	QVariantMap lObject = pVideo.toMap();

	Video * lVideo;

	QVariant lId = lObject.value("_id");
	auto iter = std::find_if(mVideos.begin(), mVideos.end(), [&lId] (IVideo * v) { return v->id() == lId; });
	if (iter == mVideos.end())
	{
		lVideo = new Video(*this, lId);
		mVideos.push_back(lVideo);
	}
	else
	{
		lVideo = static_cast<Video*>(*iter);
	}

	lVideo->updateFromVariant(pVideo);
	return lVideo;
}

} // namespace twitchtv3
