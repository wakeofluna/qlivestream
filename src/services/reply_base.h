#ifndef REPLIES_REPLY_BASE_H_
#define REPLIES_REPLY_BASE_H_

#include <QString>
#include <QVariantMap>
class QNetworkReply;


class ReplyBase
{
public:
	virtual ~ReplyBase();

	virtual QString tag() const = 0;

	inline bool hasError() const { return !mLastError.isEmpty(); }
	inline QString lastError() const { return mLastError; }
	inline int networkError() const { return mNetworkError; }

protected:
	ReplyBase(QNetworkReply & pReply);

	void setError(QString pError);
	bool checkNetworkStatus();
	QVariantMap parseJsonReply();

	QNetworkReply & mReply;

private:
	int     mNetworkError;
	QString mLastError;
};

#endif // REPLIES_REPLY_BASE_H_
