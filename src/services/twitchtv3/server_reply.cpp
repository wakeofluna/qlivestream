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

bool ServerReply::parse()
{
	if (!checkNetworkStatus())
		return false;

	QByteArray lApi = mReply.rawHeader("X-API-Version");
	if (lApi != "3")
	{
		setError("Invalid/unknown API version");
		return false;
	}

	QVariantMap lResponse = parseJsonReply();
	if (lResponse.isEmpty())
		return false;

	mData = lResponse;
	return true;
}

} // namespace twitchtv3
