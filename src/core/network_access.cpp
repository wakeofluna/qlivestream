#include "config.h"
#include "network_access.h"
#include <QAuthenticator>
#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QNetworkProxyFactory>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSignalMapper>
#include <QString>
#include <QUrl>
#include <QVariantMap>
#include "i_profile.h"

QNetworkAccessManager * NetworkAccess::mNetworkAccessManager = nullptr;

void NetworkAccess::networkGet(QNetworkRequest const & pRequest, Receiver && pReceiver, int pRedirection) const
{
	QNetworkReply * lReply = mNetworkAccessManager->get(pRequest);
	QUrl pRequestUrl = pRequest.url();

	QSignalMapper * lMapper = new QSignalMapper(lReply);
	lMapper->setMapping(lReply, lReply);
	QObject::connect(lReply, &QNetworkReply::finished, lMapper, (void (QSignalMapper::*) ()) &QSignalMapper::map);
	QObject::connect(lMapper, (void (QSignalMapper::*)(QObject*)) &QSignalMapper::mapped, [this,pRequestUrl,CAPTURE(pReceiver),pRedirection] (QObject * o) mutable
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

void NetworkAccess::networkPost(QNetworkRequest const & pRequest, QByteArray const & pBytes, Receiver && pReceiver) const
{
	QNetworkReply * lReply = mNetworkAccessManager->post(pRequest, pBytes);

	QSignalMapper * lMapper = new QSignalMapper(lReply);
	lMapper->setMapping(lReply, lReply);
	QObject::connect(lReply, &QNetworkReply::finished, lMapper, (void (QSignalMapper::*) ()) &QSignalMapper::map);
	QObject::connect(lMapper, (void (QSignalMapper::*)(QObject*)) &QSignalMapper::mapped, [this,CAPTURE(pReceiver)] (QObject * o) mutable
	{
		QNetworkReply * lReply = qobject_cast<QNetworkReply*>(o);
		lReply->deleteLater();

		pReceiver(*lReply);
	});
}

void NetworkAccess::networkPut(QNetworkRequest const & pRequest, QByteArray const & pBytes, Receiver && pReceiver) const
{
	QNetworkReply * lReply = mNetworkAccessManager->put(pRequest, pBytes);

	QSignalMapper * lMapper = new QSignalMapper(lReply);
	lMapper->setMapping(lReply, lReply);
	QObject::connect(lReply, &QNetworkReply::finished, lMapper, (void (QSignalMapper::*) ()) &QSignalMapper::map);
	QObject::connect(lMapper, (void (QSignalMapper::*)(QObject*)) &QSignalMapper::mapped, [this,CAPTURE(pReceiver)] (QObject * o) mutable
	{
		QNetworkReply * lReply = qobject_cast<QNetworkReply*>(o);
		lReply->deleteLater();

		pReceiver(*lReply);
	});
}

void NetworkAccess::networkDelete(QNetworkRequest const & pRequest, Receiver && pReceiver) const
{
	QNetworkReply * lReply = mNetworkAccessManager->deleteResource(pRequest);

	QSignalMapper * lMapper = new QSignalMapper(lReply);
	lMapper->setMapping(lReply, lReply);
	QObject::connect(lReply, &QNetworkReply::finished, lMapper, (void (QSignalMapper::*) ()) &QSignalMapper::map);
	QObject::connect(lMapper, (void (QSignalMapper::*)(QObject*)) &QSignalMapper::mapped, [this,CAPTURE(pReceiver)] (QObject * o) mutable
	{
		QNetworkReply * lReply = qobject_cast<QNetworkReply*>(o);
		lReply->deleteLater();

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
