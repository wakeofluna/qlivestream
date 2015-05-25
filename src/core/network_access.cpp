#include "config.h"
#include "network_access.h"
#include "core/profile.h"

#include <QAuthenticator>
#include <QList>
#include <QMutex>
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QNetworkProxyFactory>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSignalMapper>
#include <QString>
#include <QUrl>
#include <QVariantMap>

QNetworkAccessManager * NetworkAccess::mNetworkAccessManager = nullptr;

void NetworkAccess::networkGet(QNetworkRequest const & pRequest, Receiver && pReceiver, int pRedirection) const
{
	QNetworkReply * lReply = mNetworkAccessManager->get(pRequest);
	QUrl pRequestUrl = pRequest.url();

	QSignalMapper * lMapper = new QSignalMapper(lReply);
	lMapper->setMapping(lReply, lReply);
	QObject::connect(lReply, SIGNAL(finished()), lMapper, SLOT(map()));

	QObject::connect(lMapper, static_cast<void (QSignalMapper::*)(QObject*)>(&QSignalMapper::mapped), [this,pRequestUrl,CAPTURE(pReceiver),pRedirection] (QObject * o) mutable
	{
		QNetworkReply * lReply = qobject_cast<QNetworkReply*>(o);
		lReply->deleteLater();

		QVariant lRedirect = lReply->attribute(QNetworkRequest::RedirectionTargetAttribute);
		if (lRedirect.isValid() && pRedirection < 5)
		{
			QUrl lLocation = pRequestUrl.resolved(lRedirect.toUrl());
			if (lLocation.isValid())
			{
				QNetworkRequest lRequest(lLocation);
				networkGet(lRequest, std::move(pReceiver), pRedirection + 1);
				return;
			}
		}

		pReceiver(*lReply);
	});
}

void NetworkAccess::proxyAuthenticationRequired(QNetworkProxy const & proxy, QAuthenticator * authenticator)
{
	qDebug() << "Proxy authentication required!";
}

void NetworkAccess::sslErrors(QNetworkReply * reply, QList<QSslError> const & errors)
{
	qWarning() << "SSL errors:";
	for (QSslError const & lError : errors)
		qWarning() << '-' << lError.errorString();
}

void NetworkAccess::initialize()
{
	Q_ASSERT_X(mNetworkAccessManager == nullptr, "NetworkAccess::initialize", "network was already initialised!");
	mNetworkAccessManager = new QNetworkAccessManager();

	QNetworkProxyFactory::setUseSystemConfiguration(true);
	QObject::connect(mNetworkAccessManager, &QNetworkAccessManager::proxyAuthenticationRequired, &NetworkAccess::proxyAuthenticationRequired);
	QObject::connect(mNetworkAccessManager, &QNetworkAccessManager::sslErrors, &NetworkAccess::sslErrors);
}

void NetworkAccess::finalize()
{
	delete mNetworkAccessManager;
	mNetworkAccessManager = nullptr;
}
