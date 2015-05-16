#include "config.h"
#include "reply_base.h"
#include "core/network_access.h"

#include <QByteArray>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

ReplyBase::ReplyBase(QNetworkReply & pReply) : mReply(pReply), mNetworkError(0)
{
}

ReplyBase::~ReplyBase()
{
}

void ReplyBase::setError(QString pError)
{
	if (mLastError.isEmpty())
		NetworkAccess::networkLogError(tag(), pError);

	mLastError = pError;
}

#define ERR(x,s) case QNetworkReply::x: setError(s); break
bool ReplyBase::checkNetworkStatus()
{
	QNetworkReply::NetworkError lNetworkError = mReply.error();
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

QVariantMap ReplyBase::parseJsonReply()
{
	QVariantMap lResponse;

	QByteArray lBytes = mReply.readAll();

	QJsonParseError lError;
	lResponse = QJsonDocument::fromJson(lBytes, &lError).object().toVariantMap();
	if (lError.error != QJsonParseError::NoError)
		setError(lError.errorString());

	if (!hasError())
		NetworkAccess::networkLogMessage(tag(), lResponse);

	return lResponse;
}
