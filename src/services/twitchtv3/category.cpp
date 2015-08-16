#include "category.h"

#include <qglobal.h>
#include <QList>
#include <QMetaType>
#include <QNetworkRequest>
#include <QString>
#include <QUrl>
#include <QUrlQuery>
#include "channel.h"
#include "profile.h"
#include "server_reply.h"

#include "../../core/logger.h"
#include "../../core/reply_base.h"
#include "../../misc.h"

namespace twitchtv3
{

Category::Category(Profile & pProfile, QString pName) : ICategory(pProfile)
{
	mName = pName;
	mGiantBombId = -1;
}

Category::~Category()
{
}

Profile & Category::profile() const
{
	return static_cast<Profile&>(mProfile);
}

QString Category::logoCacheString() const
{
	return ICategory::logoCacheString(QString("twitchtv3:%1").arg(mName));
}

void Category::rollupChannels()
{
	if (!mCanRollupChannels)
		return;

	mCanRollupChannels = false;

	QUrlQuery lUrlQuery;
	lUrlQuery.addQueryItem("game", name());
	lUrlQuery.addQueryItem("limit", "25");
	lUrlQuery.addQueryItem("offset", QString::number(qMax(0, mChannels.count() - 5)));

	QUrl lUrl = profile().krakenUrl("/streams");
	lUrl.setQuery(lUrlQuery);

	QNetworkRequest lRequest = profile().serviceRequest(false);
	lRequest.setUrl(lUrl);

	Logger::StatusMessage lStatus("Getting online streams for category...");

	profile().throttledGet(lRequest, [this, lStatus] (QNetworkReply & pReply)
	{
		twitchtv3::ServerReply lReply(&profile(), pReply, "CategoryChannels");
		if (lReply.hasError())
			return;

		QVariantList lList = lReply.data().value("streams").toList();
		if (!lList.isEmpty())
		{
			for (QVariant & lStream : lList)
			{
				Channel * lChannel = profile().processChannel(lStream);
				if (lChannel != nullptr && !mChannels.contains(lChannel))
				{
					mChannels.append(lChannel);
					mCanRollupChannels = true;
				}
			}

			std::sort(mChannels.begin(), mChannels.end());
			emit channelsUpdated();
		}
	});
}

QUrl Category::categoryUrl(UrlType pType) const
{
	switch (pType)
	{
		case URL_LOGO:
			return mLogoUrl;

		case URL_CATEGORY:
			return QUrl(QString("http://www.twitch.tv/directory/game/%1").arg(name()));
	}

	return QUrl();
}

void Category::setFollowed(bool pFollow)
{
	if (profile().hasScope(AuthScope::user_follows_edit))
	{
		QNetworkRequest lRequest = profile().serviceRequest(true);
		lRequest.setUrl(profile().apiUrl(QString("/users/%1/follows/games/%2").arg(profile().account()).arg(name())));

		if (pFollow)
		{
			profile().throttledPut(lRequest, QByteArray(), [this] (QNetworkReply & pReply)
			{
				ServerReply lReply(&profile(), pReply, "FollowCategory");
				if (lReply.hasError())
					return;

				if (updateIfChanged<Flag>(mFlags, Flag::FOLLOWED, true))
					emit infoUpdated();

				updateFromVariant(lReply.data());
			});
		}
		else
		{
			profile().throttledDelete(lRequest, [this] (QNetworkReply & pReply)
			{
				ReplyBase lReply(&profile(), pReply, "UnfollowCategory");

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

void Category::updateFromVariant(QVariant pData)
{
	QVariantMap lItem = pData.toMap();
	QVariantMap lGame = lItem.value("game").toMap();
	if (lGame.isEmpty())
		return;

	if (mName != lGame.value("name").toString())
		return;

	bool lOk;
	bool lChanged = false;
	lChanged |= updateIfChanged(mNumChannels, lItem.value("channels").toInt(&lOk), lOk);
	lChanged |= updateIfChanged(mNumViewers, lItem.value("viewers").toInt(&lOk), lOk);
	lChanged |= updateIfChanged(mId, QVariant(lGame.value("_id").toInt(&lOk)), lOk);
	lChanged |= updateIfChanged(mGiantBombId, lGame.value("giantbomb_id").toInt(&lOk), lOk);

	QVariantMap lBox = lGame.value("box").toMap();
	lChanged |= updateIfChanged(mLogoUrl, lBox.value("medium").toUrl());

	if (lChanged)
		emit infoUpdated();
}

void Category::updateFlag(Flag pFlag, bool pEnabled)
{
	if (updateIfChanged<Flag>(mFlags, pFlag, pEnabled))
		emit infoUpdated();
}

} // namespace twitchtv3
