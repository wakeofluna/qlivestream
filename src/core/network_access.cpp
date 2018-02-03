#include "config.h"
#include "network_access.h"
#include <QAuthenticator>
#include <QEventLoop>
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

QNetworkReply * NetworkAccess::networkGetSync(QNetworkRequest const & pRequest) const
{
	QNetworkReply * lReply = mNetworkAccessManager->get(pRequest);

	QEventLoop lLoop;
	QObject::connect(lReply, &QNetworkReply::finished, &lLoop, &QEventLoop::quit);
	lLoop.exec();

	lReply->deleteLater();
	return lReply;
}

QNetworkReply * NetworkAccess::networkGetAsync(QNetworkRequest const & pRequest) const
{
	return mNetworkAccessManager->get(pRequest);
}

void NetworkAccess::networkGet(QNetworkRequest const & pRequest, Receiver && pReceiver) const
{
	QNetworkReply * lReply = mNetworkAccessManager->get(pRequest);

	QObject::connect(lReply, &QNetworkReply::finished, [this,lReply,CAPTURE(pReceiver)] ()
	{
		lReply->deleteLater();
		pReceiver(*lReply);
	});
}

void NetworkAccess::networkPost(QNetworkRequest const & pRequest, QByteArray const & pBytes, Receiver && pReceiver) const
{
	QNetworkReply * lReply = mNetworkAccessManager->post(pRequest, pBytes);

	QObject::connect(lReply, &QNetworkReply::finished, [this,lReply,CAPTURE(pReceiver)] ()
	{
		lReply->deleteLater();
		pReceiver(*lReply);
	});
}

void NetworkAccess::networkPut(QNetworkRequest const & pRequest, QByteArray const & pBytes, Receiver && pReceiver) const
{
	QNetworkReply * lReply = mNetworkAccessManager->put(pRequest, pBytes);

	QObject::connect(lReply, &QNetworkReply::finished, [this,lReply,CAPTURE(pReceiver)] ()
	{
		lReply->deleteLater();
		pReceiver(*lReply);
	});
}

void NetworkAccess::networkDelete(QNetworkRequest const & pRequest, Receiver && pReceiver) const
{
	QNetworkReply * lReply = mNetworkAccessManager->deleteResource(pRequest);

	QObject::connect(lReply, &QNetworkReply::finished, [this,lReply,CAPTURE(pReceiver)] ()
	{
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
