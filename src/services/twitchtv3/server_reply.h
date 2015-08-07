#ifndef SERVICES_TWITCHTV3_SERVER_REPLY_H_
#define SERVICES_TWITCHTV3_SERVER_REPLY_H_

#include <QString>
#include <QVariantMap>

#include "../../core/reply_base.h"

namespace twitchtv3
{

class Profile;
class ServerReply : public ReplyBase
{
public:
	ServerReply(Profile & pProfile, QNetworkReply & pReply, QString pTag);
	virtual ~ServerReply();

	Profile & profile() const;
	inline QVariantMap const & data() const { return mData; }

protected:
	virtual void log() const override;

	bool parse();
	QVariantMap mData;
};

} // namespace twitchtv3

#endif // SERVICES_TWITCHTV3_SERVER_REPLY_H_
