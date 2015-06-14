#include "config.h"
#include "server_reply.h"

#include <QByteArray>
#include <QNetworkReply>

namespace twitchtv3
{

ServerReply::ServerReply(QNetworkReply & pReply) : ReplyBase(pReply)
{

}

ServerReply::~ServerReply()
{

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

ServerReplySimple::ServerReplySimple(QNetworkReply & pReply, QString pTag) : ServerReply(pReply), mTag(pTag)
{
	parse();
}

} // namespace twitchtv3
