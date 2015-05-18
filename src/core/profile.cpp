#include "config.h"
#include "profile.h"
#include "core/category_object.h"
#include "core/channel_object.h"
#include "core/reply_binary.h"

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

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
		pCallback(lReply.data());
	});
}

void Profile::updateToken(QString pToken)
{
	QSqlQuery q;
	q.prepare("update profile set token=? where id=?");
	q.bindValue(0, pToken);
	q.bindValue(1, mId);
	SQL_EXEC(q);
	mToken = pToken;
}
