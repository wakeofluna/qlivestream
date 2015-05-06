#include "config.h"
#include "network_access.h"
#include "core/profile.h"

#include <QMutex>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSignalMapper>
#include <QUrl>

QNetworkAccessManager * NetworkAccess::mNetwork = nullptr;

NetworkAccess::NetworkAccess()
{
	static QMutex lMutex;

	if (mNetwork == nullptr)
	{
		QMutexLocker lLock(&lMutex);
		if (mNetwork == nullptr)
			mNetwork = new QNetworkAccessManager();
	}
}

NetworkAccess::~NetworkAccess()
{
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

void NetworkAccess::networkGet(QNetworkRequest const & pRequest, QObject * pReceiver, char const * pSlotCallback) const
{
	QNetworkReply * lReply = mNetwork->get(pRequest);

	QSignalMapper * lMapper = new QSignalMapper(lReply);
	lMapper->setMapping(lReply, lReply);
	QObject::connect(lReply, SIGNAL(finished()), lMapper, SLOT(map()));
	QObject::connect(lMapper, SIGNAL(mapped(QObject*)), pReceiver, pSlotCallback);
}
