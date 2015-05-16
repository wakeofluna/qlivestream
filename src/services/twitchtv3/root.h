#ifndef SERVICES_TWITCHTV3_ROOT_H_
#define SERVICES_TWITCHTV3_ROOT_H_

#include "../reply_base.h"
#include "auth_scope.h"

namespace twitchtv3
{

class Root : public ReplyBase
{
public:
	Root(QNetworkReply & pReply);
	~Root();

	inline bool valid() const { return mValid; }
	inline QString username() const { return mUsername; }
	inline AuthScopes const & scopes() const { return mScopes; }

private:
	bool mValid;
	QString mUsername;
	AuthScopes mScopes;
};

} // namespace twitchtv3

#endif // SERVICES_TWITCHTV3_ROOT_H_
