#ifndef SERVICES_TWITCHTV3_CHAT_CHANNEL_H_
#define SERVICES_TWITCHTV3_CHAT_CHANNEL_H_

#include "core/channel_chat.h"
#include "chat_server.h"
#include "channel_chatter.h"

namespace twitchtv3
{

class Channel;
class ChatChannel : public ::ChannelChat
{
Q_OBJECT

public:
	ChatChannel(Channel & pChannel, ChatServer::Ptr pServer);
	~ChatChannel();

	Channel & channel() const;

	void connectToChat() override;
	void disconnectFromChat(QString pMessage) override;
	void sendMessage(QString pMessage) override;

	ChannelChatter * findChatter(QStringRef pUsername, bool pOld = false) const;

private:
	friend class ChatServer;
	void onUserstate(ChatServer::Tags const & pTags);
	void onRoomstate(ChatServer::Tags const & pTags);
	void onJoin(QString pSource, ChatServer::Tags const * pTags = nullptr);
	void onPart(QString pSource, QString pMessage);
	void onMode(QString pTarget, bool pAdd, ChannelChatter::Flag pFlag);
	void onPrivmsg(QString pSource, ChatServer::Tags const & pTags, QString pMessage);

	static QStringRef usernameFrom(QString pSource);

	ChatServer::Ptr mServer;

	// USERSTATE
	QString mUserColor;
	QString mUserDisplayName;
	QVector<int> mUserEmoteSets;
	bool mUserSub;
	bool mUserTurbo;
	QString mUserType;

	// ROOMSTATE
	QString mRoomLanguage;
	bool mRoomR9K;
	bool mRoomSlow;
	bool mRoomSubsOnly;
};

} // namespace twitchtv3

#endif // SERVICES_TWITCHTV3_CHAT_CHANNEL_H_
