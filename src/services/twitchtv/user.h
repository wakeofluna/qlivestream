#ifndef SERVICES_TWITCHTV_USER_H_
#define SERVICES_TWITCHTV_USER_H_

#include <QString>

#include "../../core/i_user.h"

namespace twitchtv
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
	void updateFromChannel(QVariantMap pMap);
	void updateFromUserInfo(QVariantMap pMap);

private:
	IChannel * newChannel();
	IChannelUser * newChannelUser(IChannel & pChannel);
};

} // namespace twitchtv

#endif // SERVICES_TWITCHTV_USER_H_
