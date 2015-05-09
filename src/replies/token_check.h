#ifndef REPLIES_TOKEN_CHECK_H_
#define REPLIES_TOKEN_CHECK_H_

#include "reply_base.h"
#include "core/auth_scope.h"
#include <functional>
#include <QScopedPointer>
#include <QString>

class NetworkAccess;
class Profile;
class QObject;

class TokenCheck : public ReplyBase
{
public:
	typedef QScopedPointer<TokenCheck> Ptr;
	typedef std::function<void (Ptr &)> Receiver;

public:
	virtual ~TokenCheck();

	static void request(NetworkAccess & pNetworkAccess, Profile & pProfile, Receiver && pReceiver);

	inline bool isValid() const { return mValid; }
	inline QString username() const { return mUsername; }
	inline AuthScopes const & scopes() const { return mScopes; }

protected:
	TokenCheck();
	TokenCheck(QString pError);

	bool mValid;
	QString mUsername;
	AuthScopes mScopes;
};


#endif // REPLIES_TOKEN_CHECK_H_
