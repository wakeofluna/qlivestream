#ifndef SERVICES_TWITCHTV3_SERVER_REPLY_H_
#define SERVICES_TWITCHTV3_SERVER_REPLY_H_

#include "../reply_base.h"
#include <QVariantMap>

namespace twitchtv3
{

class ServerReply : public ReplyBase
{
public:
	ServerReply(QNetworkReply & pReply);
	virtual ~ServerReply();

protected:
	bool parse();

	QVariantMap mData;
};

} // namespace twitchtv3

#endif // SERVICES_TWITCHTV3_SERVER_REPLY_H_
