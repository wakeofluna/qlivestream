#include "config.h"
#include "reply_base.h"

#include <QByteArray>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

QVariantMap ReplyBase::parseJsonReply(QNetworkReply & pReply)
{
	QByteArray lBytes = pReply.readAll();

	QJsonParseError lError;
	QVariantMap lResponse = QJsonDocument::fromJson(lBytes, &lError).object().toVariantMap();
	if (lError.error != QJsonParseError::NoError)
		setError(lError.errorString());

	return lResponse;
}
