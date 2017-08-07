#ifndef SERVICES_TWITCHTV_CHANNEL_CHAT_H_
#define SERVICES_TWITCHTV_CHANNEL_CHAT_H_

#include <QString>
#include <QVector>
#include "channel_user.h"
#include "chat_server.h"

#include "../../core/i_channel_chat.h"
#include "../../core/i_channel_user.h"

namespace twitchtv
{

class Channel;
class ChannelChat : public IChannelChat
{
public:
	ChannelChat(Channel & pChannel, ChatServer::Ptr pServer);
	~ChannelChat();

	Profile & profile() const;
	User & owner() const;
	Channel & channel() const;
	QString name() const;

	void connectToChat() override;
	void disconnectFromChat(QString pMessage) override;
	void sendMessage(QString pMessage) override;

	ChannelUser * findChatter(QStringRef pUsername, bool pCreate = true) const;

private:
	friend class ChatServer;
	void onUserstate(ChatServer::Tags const & pTags);
	void onRoomstate(ChatServer::Tags const & pTags);
	void onJoin(QString pSource);
	void onPart(QString pSource, QString pMessage);
	void onMode(QString pTarget, bool pAdd, ChannelUser::Flag pFlag);
	void onPrivmsg(QString pSource, ChatServer::Tags const & pTags, QString pMessage);

	static QStringRef usernameFrom(QString pSource);

	ChatServer::Ptr mServer;
	ChannelUser * mSelf;

	// USERSTATE
	QVector<int> mUserEmoteSets;

	// ROOMSTATE
	QString mRoomLanguage;
	bool mRoomR9K;
	bool mRoomSlow;
	bool mRoomSubsOnly;
};

} // namespace twitchtv

#endif // SERVICES_TWITCHTV_CHANNEL_CHAT_H_
