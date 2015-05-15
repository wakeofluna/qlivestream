#ifndef SERVICES_TWITCHTV3_ROOT_H_
#define SERVICES_TWITCHTV3_ROOT_H_

#include "../reply_base.h"

namespace twitchtv3
{

class Root : public ReplyBase
{
public:
	Root(QNetworkReply & pReply);
	~Root();

	inline bool valid() const { return mValid; }
	inline QString username() const { return mUsername; }

private:
	bool mValid;
	QString mUsername;
};

} // namespace twitchtv3

#endif // SERVICES_TWITCHTV3_ROOT_H_
