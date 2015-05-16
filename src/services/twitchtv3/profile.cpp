#include "config.h"
#include "profile.h"
#include "root.h"
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

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
	QUrl lUrl = serviceUrl();

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

QUrl Profile::serviceUrl() const
{
	QUrl lUrl;
	lUrl.setScheme("https");
	lUrl.setHost("api.twitch.tv");
	lUrl.setPath("/kraken");
	return lUrl;
}

QNetworkRequest Profile::serviceRequest() const
{
	QNetworkRequest lRequest;
	lRequest.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
	lRequest.setHeader(QNetworkRequest::UserAgentHeader, APP_NAME);
	lRequest.setRawHeader("Accept", "application/vnd.twitchtv3+json");

	if (level() != ANONYMOUS && !token().isEmpty())
	{
		QString lAuth = QString("OAuth %1").arg(token());
		lRequest.setRawHeader("Authorization", lAuth.toUtf8());
	}

	return lRequest;
}


} // namespace twitchtv3
