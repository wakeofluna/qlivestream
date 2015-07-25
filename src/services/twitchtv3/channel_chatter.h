#ifndef SERVICES_TWITCHTV3_CHANNEL_CHATTER_H_
#define SERVICES_TWITCHTV3_CHANNEL_CHATTER_H_

#include "core/channel_chatter.h"
#include "chat_server.h"

namespace twitchtv3
{

class ChatChannel;
class ChannelChatter : public ::ChannelChatter
{
public:
	ChannelChatter(ChatChannel & pChannel, QString pName);
	~ChannelChatter();

	bool updateFromTags(ChatServer::Tags const & pTags);

private:
	friend class ChatChannel;

	static Color parseColor(QString pInput);

	QString mUserType;
};

} // namespace twitchtv3

#endif // SERVICES_TWITCHTV3_CHANNEL_CHATTER_H_
