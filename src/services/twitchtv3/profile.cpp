#include "config.h"
#include "profile.h"
#include "root.h"
#include "games_top.h"
#include "user_follows_channels.h"
#include "user_follows_games.h"

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>

namespace twitchtv3
{

Profile::Profile()
{
}

Profile::~Profile()
{
}

QUrl Profile::acquireTokenUrl() const
{
	AuthScopes lRequested;
	//lRequested.set(AuthScope::user_read);
	lRequested.set(AuthScope::user_blocks_edit);
	lRequested.set(AuthScope::user_blocks_read);
	lRequested.set(AuthScope::user_follows_edit);
	lRequested.set(AuthScope::user_subscriptions);
	lRequested.set(AuthScope::chat_login);
	if (level() >= MODERATOR)
	{
		lRequested.set(AuthScope::channel_check_subscription);
	}
	if (level() >= STREAMER)
	{
		lRequested.set(AuthScope::channel_subscriptions);
		lRequested.set(AuthScope::channel_stream);
		lRequested.set(AuthScope::channel_commercial);
		lRequested.set(AuthScope::channel_editor);
		lRequested.set(AuthScope::channel_read);
	}

	QUrl lUrl;
	lUrl.setScheme("https");
	lUrl.setHost("kitsune.astralkey.nl");
	lUrl.setPath("/qlivestream/twitch/");
	lUrl.setFragment(QString("scope=%1").arg(lRequested.toString()));

	return lUrl;
}

void Profile::performLogin(DefaultCallback && pCallback)
{
	QUrl lUrl = krakenUrl();

	QNetworkRequest lRequest = serviceRequest();
	lRequest.setUrl(lUrl);

	networkGet(lRequest, [this,CAPTURE(pCallback)] (QNetworkReply & pReply)
	{
		mLastError.clear();

		twitchtv3::Root lReply(pReply);
		if (lReply.hasError())
			mLastError = lReply.lastError();
		else if (lReply.valid() && lReply.username() == account())
		{
			mLoggedIn = true;
			mScopes = lReply.scopes();
		}

		pCallback();
	});
}

void Profile::getFollowedCategories(int pStart, int pLimit, CategoryCallback && pCallback)
{
	QUrl lUrl = apiUrl(QString("/users/%1/follows/games/live").arg(account()));

	QNetworkRequest lRequest = serviceRequest(false);
	lRequest.setUrl(lUrl);

	networkGet(lRequest, [this, pStart, pLimit, CAPTURE(pCallback)] (QNetworkReply & pReply)
	{
		QList<CategoryObject*> lList;

		twitchtv3::UserFollowsGames lFollowed(pReply);
		if (lFollowed.hasError())
			mLastError = lFollowed.lastError();
		else
			lList = lFollowed.createList();

		pCallback(std::move(lList));
	});
}

void Profile::getTopCategories(int pStart, int pLimit, CategoryCallback && pCallback)
{
	QUrlQuery lUrlQuery;
	lUrlQuery.addQueryItem("limit", QString::number(pLimit));
	lUrlQuery.addQueryItem("offset", QString::number(pStart));

	QUrl lUrl = krakenUrl("/games/top");
	lUrl.setQuery(lUrlQuery);

	QNetworkRequest lRequest = serviceRequest(false);
	lRequest.setUrl(lUrl);

	networkGet(lRequest, [this,CAPTURE(pCallback)] (QNetworkReply & pReply)
	{
		QList<CategoryObject*> lList;

		twitchtv3::GamesTop lGames(pReply);
		if (lGames.hasError())
			mLastError = lGames.lastError();
		else
			lList = lGames.createList();

		pCallback(std::move(lList));
	});
}

void Profile::getFollowedChannels(int pStart, int pLimit, ChannelCallback && pCallback)
{
	QUrlQuery lUrlQuery;
	lUrlQuery.addQueryItem("limit", QString::number(pLimit));
	lUrlQuery.addQueryItem("offset", QString::number(pStart));

	QUrl lUrl = krakenUrl(QString("/users/%1/follows/channels").arg(mAccount));
	lUrl.setQuery(lUrlQuery);

	QNetworkRequest lRequest = serviceRequest(false);
	lRequest.setUrl(lUrl);

	networkGet(lRequest, [this,CAPTURE(pCallback)] (QNetworkReply & pReply)
	{
		QList<ChannelObject*> lList;

		twitchtv3::UserFollowsChannels lFollows(pReply);
		if (lFollows.hasError())
			mLastError = lFollows.lastError();
		else
			lList = lFollows.createList();

		pCallback(std::move(lList));
	});
}

void Profile::getCategoryChannels(CategoryObject * pCategory, int pStart, int pLimit, ChannelCallback && pCallback)
{
	QList<ChannelObject*> lList;
	pCallback(std::move(lList));
}

QUrl Profile::apiUrl(QString pAppend) const
{
	QUrl lUrl;
	lUrl.setScheme("https");
	lUrl.setHost("api.twitch.tv");
	lUrl.setPath(QString("/api%1").arg(pAppend));
	return lUrl;
}

QUrl Profile::krakenUrl(QString pAppend) const
{
	QUrl lUrl;
	lUrl.setScheme("https");
	lUrl.setHost("api.twitch.tv");
	lUrl.setPath(QString("/kraken%1").arg(pAppend));
	return lUrl;
}

QNetworkRequest Profile::serviceRequest(bool pAuthed) const
{
	QNetworkRequest lRequest;
	lRequest.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
	lRequest.setHeader(QNetworkRequest::UserAgentHeader, APP_NAME);
	lRequest.setRawHeader("Accept", "application/vnd.twitchtv3+json");

	if (level() != ANONYMOUS && !token().isEmpty() && pAuthed)
	{
		QString lAuth = QString("OAuth %1").arg(token());
		lRequest.setRawHeader("Authorization", lAuth.toUtf8());
	}

	return lRequest;
}


} // namespace twitchtv3
