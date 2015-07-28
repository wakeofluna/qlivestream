#ifndef SERVICES_TWITCHTV3_USER_H_
#define SERVICES_TWITCHTV3_USER_H_

#include <QString>

#include "../../core/i_user.h"

namespace twitchtv3
{

class Profile;
class User : public IUser
{
public:
	User(Profile & pProfile, QString pName);
	~User();

	Profile & profile() const;

	void befriend();
	void unfriend();

	void updateFlag(Flag pFlag, bool pEnabled);

private:
	IChannel * newChannel();
	IChannelUser * newChannelUser(IChannel & pChannel);
};

} // namespace twitchtv3

#endif // SERVICES_TWITCHTV3_USER_H_
