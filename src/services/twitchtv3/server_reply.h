#ifndef SERVICES_TWITCHTV3_SERVER_REPLY_H_
#define SERVICES_TWITCHTV3_SERVER_REPLY_H_

#include "core/reply_base.h"
#include "profile.h"
#include <QVariantMap>

namespace twitchtv3
{

class Profile;
class ServerReply : public ReplyBase
{
public:
	ServerReply(Profile & pProfile, QNetworkReply & pReply);
	virtual ~ServerReply();

	Profile * profile() const;

protected:
	virtual void log() const override;

	bool parse();
	QVariantMap mData;
};

class ServerReplySimple : public ServerReply
{
public:
	ServerReplySimple(Profile & pProfile, QNetworkReply & pReply, QString pTag);

	inline QString tag() const override { return mTag; }
	inline QVariantMap const & data() const { return mData; }

protected:
	QString mTag;
};

} // namespace twitchtv3

#endif // SERVICES_TWITCHTV3_SERVER_REPLY_H_
