#ifndef SERVICES_TWITCHTV_CHANNEL_USER_H_
#define SERVICES_TWITCHTV_CHANNEL_USER_H_

#include <QString>
#include "chat_server.h"

#include "../../core/i_channel_user.h"

namespace twitchtv
{

class Channel;
class Profile;
class User;
class ChannelUser : public IChannelUser
{
public:
	ChannelUser(Channel & pChannel, User & pUser);
	~ChannelUser();

	Profile & profile() const;
	Channel & channel() const;
	User & user() const;

	void updateFromTags(ChatServer::Tags const & pTags);
	void updateFlag(Flag pFlag, bool pEnabled);

private:
	friend class ChannelChat;

	static Color parseColor(QString pInput);

	QString mUserType;
};

} // namespace twitchtv

#endif // SERVICES_TWITCHTV_CHANNEL_USER_H_
