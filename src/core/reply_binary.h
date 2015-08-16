#ifndef SERVICES_REPLY_BINARY_H_
#define SERVICES_REPLY_BINARY_H_

#include <QByteArray>
#include <QString>
#include "reply_base.h"

#include "../config.h"

class COREDLL ReplyBinary : public ReplyBase
{
public:
	ReplyBinary(IProfile * pProfile, QNetworkReply & pReply, QString pTag);
	~ReplyBinary();

	inline QByteArray       & data()       { return mData; }
	inline QByteArray const & data() const { return mData; }

protected:
	void log() const override;

	QByteArray mData;
};

#endif // SERVICES_REPLY_BINARY_H_
