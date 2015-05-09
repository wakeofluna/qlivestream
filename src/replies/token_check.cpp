#include "config.h"
#include "token_check.h"
#include "core/network_access.h"
#include "twitchtv3/root.h"

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QScopedPointer>
#include <QUrl>
#include <iostream>
#include <string>

TokenCheck::TokenCheck() : mValid(false)
{
}

TokenCheck::TokenCheck(QString pError) : ReplyBase(pError), mValid(false)
{
}

TokenCheck::~TokenCheck()
{
}

void TokenCheck::request(NetworkAccess & pNetworkAccess, Profile & pProfile, Receiver && pReceiver)
{
	QUrl lUrl = pNetworkAccess.networkUrl(pProfile);

	QNetworkRequest lRequest = pNetworkAccess.networkRequest(pProfile);
	lRequest.setUrl(lUrl);

	pNetworkAccess.networkGet(lRequest, [CAPTURE(pReceiver)] (QNetworkReply & pReply)
	{
		Ptr lReply;

		QByteArray lApi = pReply.rawHeader("X-API-Version");
		if (lApi == "3")
			lReply.reset(new twitchtv3::Root(pReply));
		else
			lReply.reset(new TokenCheck("invalid/unknown API reply"));

		pReceiver(lReply);
	});
}
