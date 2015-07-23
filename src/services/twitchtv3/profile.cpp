#include "config.h"
#include "profile.h"
#include "root.h"
#include "channel.h"
#include "chat_server.h"
#include "games_top.h"
#include "user_follows_channels.h"
#include "user_follows_games.h"

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QUrl>
#include <QUrlQuery>

static constexpr int MaxPending = 4;
static constexpr int PendingDelay = 1200;

namespace twitchtv3
{

Profile::Profile()
{
	mChatServer = new ChatServer();
	mPendingPoints = MaxPending;
	mPendingTimer = new QTimer();
	mPendingTimer->setInterval(PendingDelay);
	QObject::connect(mPendingTimer, &QTimer::timeout, this, &Profile::throttlePing);
}

Profile::~Profile()
{
	delete mPendingTimer;
	qDeleteAll(mPendingRequests);
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

	throttledGet(lRequest, [this,CAPTURE(pCallback)] (QNetworkReply & pReply)
	{
		mLastError.clear();

		twitchtv3::Root lReply(*this, pReply);
		if (lReply.hasError())
			mLastError = lReply.lastError();
		else if (lReply.valid() && lReply.username() == account())
		{
			mLoggedIn = true;
			mScopes = lReply.scopes();

			if (mScopes.test(AuthScope::chat_login))
				mChatServer->setLogin(account(), token());
		}

		pCallback();
	});
}

void Profile::getFollowedCategories(int pStart, int pLimit, CategoryCallback && pCallback)
{
	QUrl lUrl = apiUrl(QString("/users/%1/follows/games/live").arg(account()));

	QNetworkRequest lRequest = serviceRequest(false);
	lRequest.setUrl(lUrl);

	throttledGet(lRequest, [this, pStart, pLimit, CAPTURE(pCallback)] (QNetworkReply & pReply)
	{
		QList<CategoryObject*> lList;

		twitchtv3::UserFollowsGames lFollowed(*this, pReply);
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

	throttledGet(lRequest, [this,CAPTURE(pCallback)] (QNetworkReply & pReply)
	{
		QList<CategoryObject*> lList;

		twitchtv3::GamesTop lGames(*this, pReply);
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

	throttledGet(lRequest, [this,CAPTURE(pCallback)] (QNetworkReply & pReply)
	{
		QList<ChannelObject*> lList;

		twitchtv3::UserFollowsChannels lFollows(*this, pReply);
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

void Profile::getChannelStream(ChannelObject & pChannel)
{
	QUrl lUrl = krakenUrl(QString("/streams/%1").arg(pChannel.name()));

	QNetworkRequest lRequest = serviceRequest(false);
	lRequest.setUrl(lUrl);

	throttledGet(lRequest, [this] (QNetworkReply & pReply)
	{
		twitchtv3::ServerReplySimple lReply(*this, pReply, "StreamInfo");
	});
}

void Profile::getChannelFollowers(ChannelObject & pChannel)
{
	if (!pChannel.isOnline())
		return;

	QUrl lUrl = krakenUrl(QString("/channels/%1/follows").arg(pChannel.name()));

	QNetworkRequest lRequest = serviceRequest(false);
	lRequest.setUrl(lUrl);

	throttledGet(lRequest, [this] (QNetworkReply & pReply)
	{
		twitchtv3::ServerReplySimple lReply(*this, pReply, "ChannelFollows");
	});
}

void Profile::getChannelSubscribers(ChannelObject & pChannel)
{
	if (!pChannel.isEditor() || !pChannel.isPartnered())
		return;

	if (!pChannel.isOnline() && pChannel.numSubscribers() != -1)
		return;

	QUrl lUrl = krakenUrl(QString("/channels/%1/subscriptions").arg(pChannel.name()));

	QNetworkRequest lRequest = serviceRequest(true);
	lRequest.setUrl(lUrl);

	throttledGet(lRequest, [this] (QNetworkReply & pReply)
	{
		twitchtv3::ServerReplySimple lReply(*this, pReply, "ChannelSubscriptions");

		// status=422  message="%channel% does not have a subscription program"
	});
}

ChannelObject * Profile::getChannelFor(QString pName)
{
	Channel * lChannel = new Channel(*this, pName);
	return lChannel;
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
	lRequest.setRawHeader("Accept", "application/vnd.twitchtv.v3+json");

	if (level() != ANONYMOUS && !token().isEmpty() && pAuthed)
	{
		QString lAuth = QString("OAuth %1").arg(token());
		lRequest.setRawHeader("Authorization", lAuth.toUtf8());
	}

	return lRequest;
}

void Profile::throttledGet(QNetworkRequest const& pRequest, Receiver && pReceiver)
{
	QMutexLocker lGuard(&mPendingMutex);

	if (mPendingPoints > 0)
	{
		--mPendingPoints;
		networkGet(pRequest, std::move(pReceiver));
	}
	else
	{
		QScopedPointer<PendingRequest> lPending(new PendingRequest(pRequest, std::move(pReceiver)));
		mPendingRequests.enqueue(lPending.take());
	}

	if (!mPendingTimer->isActive())
		mPendingTimer->start();
}

void Profile::throttledPost(const QNetworkRequest& pRequest, const QByteArray& pData, Receiver&& pReceiver)
{
	QMutexLocker lGuard(&mPendingMutex);

	if (mPendingPoints > 0)
	{
		--mPendingPoints;
		networkPost(pRequest, pData, std::move(pReceiver));
	}
	else
	{
		QScopedPointer<PendingRequest> lPending(new PendingRequest(pRequest, pData, std::move(pReceiver), PendingRequest::POST));
		mPendingRequests.enqueue(lPending.take());
	}

	if (!mPendingTimer->isActive())
		mPendingTimer->start();
}

void Profile::throttledPut(const QNetworkRequest& pRequest, QByteArray const & pData, Receiver && pReceiver)
{
	QMutexLocker lGuard(&mPendingMutex);

	if (mPendingPoints > 0)
	{
		--mPendingPoints;
		networkPut(pRequest, pData, std::move(pReceiver));
	}
	else
	{
		QScopedPointer<PendingRequest> lPending(new PendingRequest(pRequest, pData, std::move(pReceiver), PendingRequest::PUT));
		mPendingRequests.enqueue(lPending.take());
	}

	if (!mPendingTimer->isActive())
		mPendingTimer->start();
}

void Profile::throttlePing()
{
	QMutexLocker lGuard(&mPendingMutex);

	if (mPendingRequests.empty())
	{
		++mPendingPoints;
		if (mPendingPoints >= MaxPending)
			mPendingTimer->stop();
	}
	else
	{
		QScopedPointer<PendingRequest> lPending(mPendingRequests.dequeue());
		switch (lPending->mType)
		{
			case PendingRequest::GET:
				networkGet(lPending->mRequest, std::move(lPending->mCallback));
				break;
			case PendingRequest::POST:
				networkPost(lPending->mRequest, lPending->mData, std::move(lPending->mCallback));
				break;
			case PendingRequest::PUT:
				networkPut(lPending->mRequest, lPending->mData, std::move(lPending->mCallback));
				break;
		}
	}
}

} // namespace twitchtv3
