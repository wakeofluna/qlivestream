#include "server_reply.h"

#include <QByteArray>
#include <QMap>
#include <QNetworkReply>
#include "profile.h"

#include "../../core/logger.h"

namespace twitchtv
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
	ReplyBase::log(mData);
}

bool ServerReply::parse()
{
	bool lOk = checkNetworkStatus();
	if (lOk)
	{
		QByteArray lApi = mReply.rawHeader("X-API-Version");
		if (lApi == "3" || lApi.isNull())
		{
			mData = parseJsonReply();
		}
		else
		{
			setError(QString("Invalid/unknown API version: '%1'").arg(QString::fromUtf8(lApi)));

			// Try it anyway..
			mData = parseJsonReply();
			if (mData.isEmpty())
				lOk = false;
		}
	}

	log();
	return lOk;
}

} // namespace twitchtv
