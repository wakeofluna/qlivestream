#include "config.h"
#include "root.h"

#include <QNetworkReply>
#include <QUrl>
#include <QVariantList>
#include "core/network_access.h"

namespace twitchtv3
{

Root::Root(QNetworkReply & pReply) : ServerReply(pReply)
{
	if (!parse())
		return;

	QVariantMap lToken = mData.value("token").toMap();
	mValid = lToken.value("valid").toBool();
	mUsername = lToken.value("user_name").toString();

	QVariantMap lTokenAuth = lToken.value("authorization").toMap();

	QVariantList lTokenScopes = lTokenAuth.value("scopes").toList();
	for (int i = 0; i < lTokenScopes.size(); ++i)
	{
		AuthScope lScope = AuthScope::fromString(lTokenScopes[i].toString());
		mScopes.set(lScope);
	}
}

Root::~Root()
{
}

QString Root::tag() const
{
	return QString("Authentication check");
}

} // namespace twitchtv3
