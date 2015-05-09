#ifndef REPLIES_TWITCHTV3_ROOT_H_
#define REPLIES_TWITCHTV3_ROOT_H_

#include "replies/token_check.h"

class QNetworkReply;

namespace twitchtv3
{

class Root : public TokenCheck
{
public:
	Root(QNetworkReply & pReply);
	~Root();

private:

};

} // namespace twitchtv3

#endif // REPLIES_TWITCHTV3_ROOT_H_
