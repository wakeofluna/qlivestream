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
	{
		qWarning() << "Communication error: " << pError;
		Logger::get()->logNetworkError(tag(), pError);
	}

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
		ERR(RemoteHostClosedError, QT_TRANSLATE_NOOP("NetworkStatus", "Remote host closed the connection"));
		ERR(HostNotFoundError, QT_TRANSLATE_NOOP("NetworkStatus", "Host not found"));
		ERR(TimeoutError, QT_TRANSLATE_NOOP("NetworkStatus", "Connection timeout"));
		ERR(OperationCanceledError, QT_TRANSLATE_NOOP("NetworkStatus", "Operation canceled"));
		ERR(SslHandshakeFailedError, QT_TRANSLATE_NOOP("NetworkStatus", "SSL handshake failed"));
		ERR(TemporaryNetworkFailureError, QT_TRANSLATE_NOOP("NetworkStatus", "Temporary network failure"));
		ERR(NetworkSessionFailedError, QT_TRANSLATE_NOOP("NetworkStatus", "Network session failed"));

		ERR(ProxyConnectionRefusedError, QT_TRANSLATE_NOOP("NetworkStatus", "Proxy connection refused"));
		ERR(ProxyConnectionClosedError, QT_TRANSLATE_NOOP("NetworkStatus", "Proxy closed the connection"));
		ERR(ProxyNotFoundError, QT_TRANSLATE_NOOP("NetworkStatus", "Proxy not found"));
		ERR(ProxyTimeoutError, QT_TRANSLATE_NOOP("NetworkStatus", "Proxy timed out"));
		ERR(ProxyAuthenticationRequiredError, QT_TRANSLATE_NOOP("NetworkStatus", "Proxy authentication required"));

		ERR(ContentAccessDenied, QT_TRANSLATE_NOOP("NetworkStatus", "Access denied"));
		ERR(ContentOperationNotPermittedError, QT_TRANSLATE_NOOP("NetworkStatus", "Operation not permitted"));
		ERR(ContentNotFoundError, QT_TRANSLATE_NOOP("NetworkStatus", "Content not found"));
		ERR(AuthenticationRequiredError, QT_TRANSLATE_NOOP("NetworkStatus", "Authentication required"));

		ERR(ProtocolInvalidOperationError, QT_TRANSLATE_NOOP("NetworkStatus", "Invalid operation"));

		ERR(InternalServerError, QT_TRANSLATE_NOOP("NetworkStatus", "Internal server error"));
		ERR(OperationNotImplementedError, QT_TRANSLATE_NOOP("NetworkStatus", "Operation not implemented"));
		ERR(ServiceUnavailableError, QT_TRANSLATE_NOOP("NetworkStatus", "Service unavailable error"));

		default: setError(QString("Network error #%1").arg(lNetworkError)); break;
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
		Logger::get()->logNetworkMessage(tag(), lResponse);

	return lResponse;
}

QByteArray ReplyBase::readByteArray()
{
	QByteArray lBytes = mReply.readAll();

	if (!hasError())
		Logger::get()->logNetworkMessage(tag(), QString("(%1 bytes)").arg(lBytes.size()));

	return lBytes;
}
