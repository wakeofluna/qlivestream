#ifndef SERVICES_REPLY_BINARY_H_
#define SERVICES_REPLY_BINARY_H_

#include "reply_base.h"
#include <QByteArray>

class ReplyBinary : public ReplyBase
{
public:
	ReplyBinary(QNetworkReply & pReply, QString pTag);
	~ReplyBinary();

	QString tag() const override;

	inline QByteArray data() const { return mData; }

protected:
	QString mTag;
	QByteArray mData;
};

#endif // SERVICES_REPLY_BINARY_H_
