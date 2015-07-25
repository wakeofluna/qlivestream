#ifndef SERVICES_TWITCHTV3_GAME_STREAMS_H_
#define SERVICES_TWITCHTV3_GAME_STREAMS_H_

#include "server_reply.h"

class ChannelObject;
template <typename T> class QList;

namespace twitchtv3
{

class Profile;
class GameStreams : public ServerReply
{
public:
	GameStreams(Profile & pProfile, QNetworkReply & pReply);
	~GameStreams();

	QString tag() const override;

	QList<ChannelObject*> createList() const;
};

} // namespace twitchtv3

#endif // SERVICES_TWITCHTV3_GAME_STREAMS_H_
