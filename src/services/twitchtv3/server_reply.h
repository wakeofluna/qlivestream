#ifndef SERVICES_TWITCHTV3_SERVER_REPLY_H_
#define SERVICES_TWITCHTV3_SERVER_REPLY_H_

#include "core/reply_base.h"
#include <QVariantMap>

namespace twitchtv3
{

class ServerReply : public ReplyBase
{
public:
	ServerReply(QNetworkReply & pReply);
	virtual ~ServerReply();

protected:
	virtual void log() const override;

	bool parse();
	QVariantMap mData;
};

class ServerReplySimple : public ServerReply
{
public:
	ServerReplySimple(QNetworkReply & pReply, QString pTag = QString());

	inline QString tag() const override { return mTag; }
	inline QVariantMap const & data() const { return mData; }

protected:
	QString mTag;
};

} // namespace twitchtv3

#endif // SERVICES_TWITCHTV3_SERVER_REPLY_H_
