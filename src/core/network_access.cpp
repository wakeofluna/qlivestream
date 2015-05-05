#include "config.h"
#include "network_access.h"
#include "core/profile.h"

#include <QMutex>

QNetworkAccessManager * NetworkAccess::mNetwork = nullptr;

NetworkAccess::NetworkAccess()
{
	static QMutex lMutex;

	QMutexLocker lLock(&lMutex);
	if (mNetwork == nullptr)
		mNetwork = new QNetworkAccessManager();
}

NetworkAccess::~NetworkAccess()
{
}

QNetworkAccessManager * NetworkAccess::network() const
{
	return mNetwork;
}

QNetworkRequest NetworkAccess::networkRequest(Profile * pProfile) const
{
	QNetworkRequest lRequest;
	lRequest.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
	lRequest.setHeader(QNetworkRequest::UserAgentHeader, "Qtwitch");
	lRequest.setRawHeader("Accept", "application/vnd.twitchtv3+json");

	if (pProfile != nullptr)
	{
		QString lAuth = QString("OAuth ") + pProfile->getToken();
		lRequest.setRawHeader("Authorization", lAuth.toUtf8());
	}

	return lRequest;
}

QUrl NetworkAccess::networkUrl() const
{
	QUrl lUrl;
	lUrl.setScheme("https");
	lUrl.setHost("api.twitch.tv");
	lUrl.setPath("/kraken");
	return lUrl;
}
