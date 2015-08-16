#include "server_reply.h"

#include <QByteArray>
#include <QMap>
#include <QNetworkReply>
#include "profile.h"

#include "../../core/logger.h"

namespace twitchtv3
{

ServerReply::ServerReply(Profile * pProfile, QNetworkReply & pReply, QString pTag) : ReplyBase(pProfile, pReply, pTag)
{
	parse();
}

ServerReply::~ServerReply()
{

}

Profile * ServerReply::profile() const
{
	return static_cast<Profile*>(mProfile);
}

void ServerReply::log() const
{
	if (hasError())
		Logger::get()->logNetworkError(tag(), lastError(), mData);
	else
		Logger::get()->logNetworkMessage(tag(), mData);
}

bool ServerReply::parse()
{
	bool lOk = checkNetworkStatus();
	if (lOk)
	{
		QByteArray lApi = mReply.rawHeader("X-API-Version");
		if (lApi == "3")
		{
			mData = parseJsonReply();
		}
		else
		{
			setError("Invalid/unknown API version");
			lOk = false;
		}
	}

	log();
	return lOk;
}

} // namespace twitchtv3
