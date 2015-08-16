#include "reply_base.h"

#include <qglobal.h>
#include <QByteArray>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>
#include <QNetworkReply>
#include "i_profile.h"


ReplyBase::ReplyBase(IProfile * pProfile, QNetworkReply & pReply, QString pTag) : mProfile(pProfile), mReply(pReply), mTag(pTag), mNetworkError(-1)
{
	checkNetworkStatus();
}

ReplyBase::~ReplyBase()
{
}

void ReplyBase::log() const
{
	if (!hasError())
		Logger::get()->logNetworkMessage(mTag, "OK");
	else
		Logger::get()->logNetworkError(mTag, lastError(), QVariant());
}

void ReplyBase::setError(QString pError)
{
	if (mLastError.isEmpty())
		qWarning() << mTag << "Communication error: " << pError;

	mLastError = pError;
	if (mProfile != nullptr)
		mProfile->setLastError(pError);
}

#define ERR(x,s) case QNetworkReply::x: setError(s); break
bool ReplyBase::checkNetworkStatus()
{
	if (mNetworkError != -1)
		return (mNetworkError == QNetworkReply::NoError);

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

		ERR(ProtocolUnknownError, QT_TRANSLATE_NOOP("NetworkStatus", "Protocol unknown"));
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

	return lResponse;
}

QByteArray ReplyBase::readByteArray()
{
	return mReply.readAll();
}
