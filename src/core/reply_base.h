#ifndef REPLIES_REPLY_BASE_H_
#define REPLIES_REPLY_BASE_H_

#include <QMetaType>
#include <QString>

#include "../config.h"

class IProfile;
class QByteArray;
class QNetworkReply;

class COREDLL ReplyBase
{
public:
	ReplyBase(IProfile * pProfile, QNetworkReply & pReply, QString pTag);
	virtual ~ReplyBase();

	inline QString tag() const { return mTag; }
	inline bool hasError() const { return !mLastError.isEmpty(); }
	inline QString lastError() const { return mLastError; }
	inline int networkError() const { return mNetworkError; }

	inline IProfile * profile() const { return mProfile; }

	virtual void log() const;

	bool checkNetworkStatus();
	QVariantMap parseJsonReply();
	QByteArray readByteArray();

protected:
	void log(QVariant pData) const;
	void setError(QString pError);

	QVariantMap addHeadersToData(QVariant pData) const;

	IProfile * mProfile;
	QNetworkReply & mReply;
	QString mTag;

private:
	int     mNetworkError;
	QString mLastError;
};

#endif // REPLIES_REPLY_BASE_H_
