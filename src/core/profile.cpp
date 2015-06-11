#include "config.h"
#include "profile.h"
#include "core/reply_binary.h"

#include <QNetworkRequest>
#include <QSqlQuery>

Profile::Profile()
{
	mLoggedIn = false;
	mId = -1;
	mLevel = STREAMER;
}

Profile::~Profile()
{
}

void Profile::downloadLogo(QUrl const & pUrl, DataCallback && pCallback)
{
	QNetworkRequest lRequest;
	lRequest.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
	lRequest.setUrl(pUrl);

	networkGet(lRequest, [this,CAPTURE(pCallback)] (QNetworkReply & pReply)
	{
		ReplyBinary lReply(pReply, "Logo");
		pCallback(std::move(lReply.data()));
	});
}

void Profile::updateToken(QString pToken)
{
	QSqlQuery q;
	SQL_PREPARE(q, "update profile set token=? where id=?");
	q.bindValue(0, pToken);
	q.bindValue(1, mId);
	SQL_EXEC(q);
	mToken = pToken;
}
