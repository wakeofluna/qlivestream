#ifndef REPLIES_REPLY_BASE_H_
#define REPLIES_REPLY_BASE_H_

#include <QString>
#include <QVariantMap>
class QNetworkReply;


class ReplyBase
{
public:
	virtual inline ~ReplyBase() {}

	inline bool hasError() const { return !mParseError.isEmpty(); }
	inline QString lastError() const { return mParseError; }

protected:
	inline ReplyBase() {}
	inline ReplyBase(QString pError) : mParseError(pError) {}
	inline void setError(QString pError) { mParseError = pError; }

	QVariantMap parseJsonReply(QNetworkReply & pReply);
	QString mParseError;
};

#endif // REPLIES_REPLY_BASE_H_
