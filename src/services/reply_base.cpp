#include "config.h"
#include "reply_base.h"
#include "core/network_access.h"

#include <QByteArray>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

#define ERR(x,s) case QNetworkReply::x: setError(s); break
bool ReplyBase::checkNetworkStatus(QNetworkReply & pReply)
{
	QNetworkReply::NetworkError lNetworkError = pReply.error();
	mNetworkError = lNetworkError;

	if (lNetworkError == QNetworkReply::NoError)
		return true;

	switch (lNetworkError)
	{
		ERR(ConnectionRefusedError, QT_TRANSLATE_NOOP("NetworkStatus", "Connection refused"));
		ERR(HostNotFoundError, QT_TRANSLATE_NOOP("NetworkStatus", "Host not found"));
		ERR(TimeoutError, QT_TRANSLATE_NOOP("NetworkStatus", "Connection timeout"));

		ERR(ProxyConnectionRefusedError, QT_TRANSLATE_NOOP("NetworkStatus", "Proxy connection refused"));

		ERR(ContentAccessDenied, QT_TRANSLATE_NOOP("NetworkStatus", "Access denied"));
		ERR(ContentNotFoundError, QT_TRANSLATE_NOOP("NetworkStatus", "Content not found"));
		ERR(AuthenticationRequiredError, QT_TRANSLATE_NOOP("NetworkStatus", "Authentication required"));

		ERR(InternalServerError, QT_TRANSLATE_NOOP("NetworkStatus", "Internal server error"));
		ERR(ServiceUnavailableError, QT_TRANSLATE_NOOP("NetworkStatus", "Service unavailable error"));

		default: setError("Network error #" + QString::number(lNetworkError)); break;
	}

	return false;
}
#undef ERR

QVariantMap ReplyBase::parseJsonReply(QString pTag, QNetworkReply & pReply)
{
	QVariantMap lResponse;

	if (checkNetworkStatus(pReply))
	{
		QByteArray lBytes = pReply.readAll();

		QJsonParseError lError;
		lResponse = QJsonDocument::fromJson(lBytes, &lError).object().toVariantMap();
		if (lError.error != QJsonParseError::NoError)
			setError(lError.errorString());
	}

	if (hasError())
		NetworkAccess::networkLogMessage(pTag, lastError());
	else
		NetworkAccess::networkLogMessage(pTag, lResponse);

	return lResponse;
}
