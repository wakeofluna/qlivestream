#ifndef SERVICES_TWITCHTV3_USER_FOLLOWS_CHANNELS_H_
#define SERVICES_TWITCHTV3_USER_FOLLOWS_CHANNELS_H_

#include "server_reply.h"

class ChannelObject;
template <typename T> class QList;

namespace twitchtv3
{

class UserFollowsChannels : public ServerReply
{
public:
	UserFollowsChannels(QNetworkReply & pReply);
	~UserFollowsChannels();

	QString tag() const override;

	QList<ChannelObject*> createList() const;
};

} // namespace twitchtv3

#endif // SERVICES_TWITCHTV3_USER_FOLLOWS_CHANNELS_H_
