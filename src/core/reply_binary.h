#ifndef SERVICES_REPLY_BINARY_H_
#define SERVICES_REPLY_BINARY_H_

#include "config.h"
#include "reply_base.h"
#include <QByteArray>

class COREDLL ReplyBinary : public ReplyBase
{
public:
	ReplyBinary(QNetworkReply & pReply, QString pTag);
	~ReplyBinary();

	QString tag() const override;

	inline QByteArray       & data()       { return mData; }
	inline QByteArray const & data() const { return mData; }

protected:
	QString mTag;
	QByteArray mData;
};

#endif // SERVICES_REPLY_BINARY_H_
