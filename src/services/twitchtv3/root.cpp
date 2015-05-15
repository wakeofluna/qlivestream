#include "config.h"
#include "root.h"

#include <QNetworkReply>
#include <QUrl>
#include <QVariantList>


namespace twitchtv3
{

Root::Root(QNetworkReply & pReply)
{
	QVariantMap lResponse = parseJsonReply(pReply);
	if (lResponse.isEmpty())
		return;

	QVariantMap lToken = lResponse.value("token").toMap();
	mValid = lToken.value("valid").toBool();
	mUsername = lToken.value("user_name").toString();

	QVariantMap lTokenAuth = lToken.value("authorization").toMap();
	QVariantList lTokenScopes = lTokenAuth.value("scopes").toList();
	/* TODO
	for (int i = 0; i < lTokenScopes.size(); ++i)
	{
		AuthScope lScope = AuthScope::fromString(lTokenScopes[i].toString());
		mScopes.set(lScope);
	}
	*/
}

Root::~Root()
{
}


} // namespace twitchtv3
