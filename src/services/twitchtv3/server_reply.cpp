#include "config.h"
#include "server_reply.h"
#include "profile.h"

#include <QByteArray>
#include <QNetworkReply>

namespace twitchtv3
{

ServerReply::ServerReply(Profile & pProfile, QNetworkReply & pReply) : ReplyBase(pProfile, pReply)
{

}

ServerReply::~ServerReply()
{

}

Profile * ServerReply::profile() const
{
	return static_cast<Profile*>(&mProfile);
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
	bool lOk = true;

	lOk &= checkNetworkStatus();

	QByteArray lApi = mReply.rawHeader("X-API-Version");
	if (lApi != "3")
	{
		setError("Invalid/unknown API version");
		lOk = false;
	}
	else
	{
		mData = parseJsonReply();
	}

	log();
	return lOk;
}

ServerReplySimple::ServerReplySimple(Profile & pProfile, QNetworkReply & pReply, QString pTag) : ServerReply(pProfile, pReply), mTag(pTag)
{
	parse();
}

} // namespace twitchtv3
