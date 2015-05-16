#ifndef REPLIES_REPLY_BASE_H_
#define REPLIES_REPLY_BASE_H_

#include <QString>
#include <QVariantMap>
class QNetworkReply;


class ReplyBase
{
public:
	virtual inline ~ReplyBase() {}

	inline bool hasError() const { return !mReplyError.isEmpty(); }
	inline QString lastError() const { return mReplyError; }

protected:
	inline ReplyBase() : mNetworkError(0) {}
	inline ReplyBase(QString pError) : mReplyError(pError), mNetworkError(0) {}
	inline void setError(QString pError) { mReplyError = pError; }

	QVariantMap parseJsonReply(QString pTag, QNetworkReply & pReply);
	QString mReplyError;
	int mNetworkError;

private:
	bool checkNetworkStatus(QNetworkReply & pReply);
};

#endif // REPLIES_REPLY_BASE_H_
