#ifndef REPLIES_REPLY_BASE_H_
#define REPLIES_REPLY_BASE_H_

#include "config.h"
#include <QString>
#include <QVariantMap>

class Profile;
class QByteArray;
class QNetworkReply;


class COREDLL ReplyBase
{
public:
	virtual ~ReplyBase();

	virtual QString tag() const = 0;

	inline bool hasError() const { return !mLastError.isEmpty(); }
	inline QString lastError() const { return mLastError; }
	inline int networkError() const { return mNetworkError; }

	inline Profile & profile() const { return mProfile; }

protected:
	ReplyBase(Profile & pProfile, QNetworkReply & pReply);

	virtual void log() const = 0;

	void setError(QString pError);
	bool checkNetworkStatus();
	QVariantMap parseJsonReply();
	QByteArray readByteArray();

	Profile & mProfile;
	QNetworkReply & mReply;

private:
	int     mNetworkError;
	QString mLastError;
};

#endif // REPLIES_REPLY_BASE_H_
