#include "profile.h"

#include <qglobal.h>
#include <QList>
#include <QMetaType>
#include <QObject>
#include <QScopedPointer>
#include <QSharedData>
#include <QTimer>
#include <QUrlQuery>
#include <QVariant>
#include "category.h"
#include "channel.h"
#include "server_reply.h"
#include "user.h"

#include "../../config.h"
#include "../../core/class_bitset.h"
#include "../../core/i_category.h"
#include "../../core/logger.h"
#include "../../core/reply_base.h"

static constexpr int MaxPending = 4;
static constexpr int PendingDelay = 1200;

namespace twitchtv3
{

Profile::Profile()
{
	mChatServer = new ChatServer(*this);
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
	if (level() >= EDITOR)
	{
		lRequested.set(AuthScope::channel_check_subscription);
		lRequested.set(AuthScope::channel_subscriptions);
		lRequested.set(AuthScope::channel_commercial);
		lRequested.set(AuthScope::channel_editor);
	}
	if (level() >= STREAMER)
	{
		lRequested.set(AuthScope::channel_stream);
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

	mLoggedIn = false;
	mScopes.reset();

	throttledGet(lRequest, [this,CAPTURE(pCallback)] (QNetworkReply & pReply)
	{
		twitchtv3::ServerReply lReply(*this, pReply, "Authentication");
		if (lReply.hasError())
			return;

		QVariantMap lToken = lReply.data().value("token").toMap();

		bool lValid = lToken.value("valid").toBool();
		QString lUsername = lToken.value("user_name").toString();

		if (lValid && lUsername == account())
		{
			mLoggedIn = true;

			QVariantMap lTokenAuth = lToken.value("authorization").toMap();
			QVariantList lTokenScopes = lTokenAuth.value("scopes").toList();
			for (int i = 0; i < lTokenScopes.size(); ++i)
			{
				AuthScope lScope = AuthScope::fromString(lTokenScopes[i].toString());
				mScopes.set(lScope);
			}
		}

		pCallback();
	});
}

// XXX -DRY- rollups for Profile
void Profile::rollupFollowedChannels()
{
	if (!mCanRollupFollowedChannels)
		return;

	mCanRollupFollowedChannels = false;

	QUrlQuery lUrlQuery;
	lUrlQuery.addQueryItem("limit", "50");
	lUrlQuery.addQueryItem("offset", QString::number(qMax(0, mFollowedChannels.count() - 5)));

	QUrl lUrl = krakenUrl(QString("/users/%1/follows/channels").arg(mAccount));
	lUrl.setQuery(lUrlQuery);

	QNetworkRequest lRequest = serviceRequest(false);
	lRequest.setUrl(lUrl);

	Logger::StatusMessage lStatus("Getting followed channels...");

	throttledGet(lRequest, [this, lStatus] (QNetworkReply & pReply)
	{
		twitchtv3::ServerReply lReply(*this, pReply, "UserFollowsChannels");
		if (lReply.hasError())
			return;

		QVariantList lList = lReply.data().value("follows").toList();
		if (!lList.isEmpty())
		{
			for (QVariant & lChannelItem : lList)
			{
				Channel * lChannel = processChannel(lChannelItem);
				if (lChannel != nullptr && !mFollowedChannels.contains(lChannel))
				{
					mFollowedChannels.append(lChannel);
					mCanRollupFollowedChannels = true;
				}
			}

			std::sort(mFollowedChannels.begin(), mFollowedChannels.end());
			emit followedChannelsUpdated();
		}
	});
}

void Profile::rollupFollowedCategories()
{
	if (!mCanRollupFollowedCategories)
		return;

	mCanRollupFollowedCategories = false;

	QUrl lUrl = apiUrl(QString("/users/%1/follows/games/live").arg(account()));

	QNetworkRequest lRequest = serviceRequest(false);
	lRequest.setUrl(lUrl);

	Logger::StatusMessage lStatus("Getting followed categories...");

	throttledGet(lRequest, [this] (QNetworkReply & pReply)
	{
		twitchtv3::ServerReply lReply(*this, pReply, "UserFollowsCategories");
		if (lReply.hasError())
			return;

		QVariantList lList = lReply.data().value("follows").toList();
		if (!lList.isEmpty())
		{
			for (QVariant & lCategoryItem : lList)
			{
				Category * lCategory = processCategory(lCategoryItem);
				lCategory->updateFlag(Category::Flag::FOLLOWED, true);

				if (lCategory != nullptr && !mFollowedCategories.contains(lCategory))
				{
					mFollowedCategories.append(lCategory);
					mCanRollupFollowedCategories = true;
				}
			}

			std::sort(mFollowedCategories.begin(), mFollowedCategories.end());
			emit followedCategoriesUpdated();
		}
	});
}

void Profile::rollupTopCategories()
{
	if (!mCanRollupTopCategories)
		return;

	mCanRollupTopCategories = false;

	QUrlQuery lUrlQuery;
	lUrlQuery.addQueryItem("limit", "25");
	lUrlQuery.addQueryItem("offset", QString::number(qMax(0, mTopCategories.count() - 5)));

	QUrl lUrl = krakenUrl("/games/top");
	lUrl.setQuery(lUrlQuery);

	QNetworkRequest lRequest = serviceRequest(false);
	lRequest.setUrl(lUrl);

	Logger::StatusMessage lStatus("Getting top categories...");

	throttledGet(lRequest, [this, lStatus] (QNetworkReply & pReply)
	{
		twitchtv3::ServerReply lReply(*this, pReply, "TopCategories");
		if (lReply.hasError())
			return;

		QVariantList lList = lReply.data().value("top").toList();
		if (!lList.isEmpty())
		{
			for (QVariant & lCategoryItem : lList)
			{
				Category * lCategory = processCategory(lCategoryItem);
				if (lCategory != nullptr && !mTopCategories.contains(lCategory))
				{
					mTopCategories.append(lCategory);
					mCanRollupTopCategories = true;
				}
			}

			std::sort(mTopCategories.begin(), mTopCategories.end());
			emit topCategoriesUpdated();
		}
	});
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

QUrl Profile::usherUrl(QString pAppend) const
{
	QUrl lUrl;
	lUrl.setScheme("http");
	lUrl.setHost("usher.twitch.tv");
	lUrl.setPath(pAppend);
	return lUrl;
}

QNetworkRequest Profile::serviceRequest(bool pAuthed, bool pJson) const
{
	QNetworkRequest lRequest;
	lRequest.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);

	if (pJson)
	{
		lRequest.setHeader(QNetworkRequest::UserAgentHeader, APP_NAME);
		lRequest.setRawHeader("Accept", "application/vnd.twitchtv.v3+json");
	}

	if (level() != ANONYMOUS && !token().isEmpty() && pAuthed)
	{
		QString lAuth = QString("OAuth %1").arg(token());
		lRequest.setRawHeader("Authorization", lAuth.toUtf8());
	}

	return lRequest;
}

QNetworkReply * Profile::synchronisedGet(QNetworkRequest const & pRequest)
{
	return networkGetSync(pRequest);
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
		mPendingRequests.enqueue(new PendingRequest(pRequest, std::move(pReceiver)));
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
		mPendingRequests.enqueue(new PendingRequest(pRequest, pData, std::move(pReceiver), PendingRequest::POST));
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
		mPendingRequests.enqueue(new PendingRequest(pRequest, pData, std::move(pReceiver), PendingRequest::PUT));
	}

	if (!mPendingTimer->isActive())
		mPendingTimer->start();
}

void Profile::throttledDelete(QNetworkRequest const & pRequest, Receiver && pReceiver)
{
	QMutexLocker lGuard(&mPendingMutex);

	if (mPendingPoints > 0)
	{
		--mPendingPoints;
		networkDelete(pRequest, std::move(pReceiver));
	}
	else
	{
		mPendingRequests.enqueue(new PendingRequest(pRequest, std::move(pReceiver), PendingRequest::DELETE));
	}

	if (!mPendingTimer->isActive())
		mPendingTimer->start();
}

Channel * Profile::processChannel(QVariant pValue)
{
	QVariantMap lValue = pValue.toMap();

	QVariantMap lCheck = lValue.value("channel", lValue).toMap();

	QString lName = lCheck.value("name").toString();
	if (lName.isEmpty())
		return nullptr;

	IUser * lIUser = getUserFor(lName);
	IChannel * lIChannel = lIUser->channel(true);

	Channel * lChannel = static_cast<Channel*>(lIChannel);
	lChannel->updateFromVariant(pValue);

	return lChannel;
}

Category * Profile::processCategory(QVariant pValue)
{
	QVariantMap lValue = pValue.toMap();

	QVariantMap lCheck = lValue.value("game", lValue).toMap();

	QString lName = lCheck.value("name").toString();
	if (lName.isEmpty())
		return nullptr;

	Category * lCategory = static_cast<Category*>(getCategoryFor(lName));
	lCategory->updateFromVariant(pValue);

	return lCategory;
}

IUser * Profile::newUserFor(QString pName)
{
	return new User(*this, pName);
}

ICategory * Profile::newCategoryFor(QString pName)
{
	return new Category(*this, pName);
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
			case PendingRequest::DELETE:
				networkDelete(lPending->mRequest, std::move(lPending->mCallback));
				break;
		}
	}
}

} // namespace twitchtv3
