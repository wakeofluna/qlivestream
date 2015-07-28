#ifndef CORE_I_CHANNEL_CHAT_H_
#define CORE_I_CHANNEL_CHAT_H_

#include <QList>
#include <QString>
#include <QVector>
#include "i_channel_user.h"

#include "../config.h"

class IChannel;

class COREDLL IChannelChat : public QObject
{
Q_OBJECT

public:
	enum State
	{
		NONE,
		JOINING,
		JOINED,
		LEAVING
	};

	struct SmileyInfo
	{
		short from;
		short to;
		QString id;
	};
	typedef QVector<SmileyInfo> SmileyList;

public:
	explicit IChannelChat(IChannel & pChannel);
	~IChannelChat();

	inline IChannel & channel() const { return mChannel; }
	inline State state() const { return mState; }
	inline QList<IChannelUser *> chatters() const { return mChatters; }
	bool hasChatter(IChannelUser const & pChatter);

public slots:
	virtual void connectToChat() = 0;
	virtual void disconnectFromChat(QString pMessage = QString()) = 0;
	virtual void sendMessage(QString pMessage) = 0;

signals:
	void stateChanged();
	void chatError(QString pMessage);

	void chatterNew(IChannelUser & pChatter);
	void chatterChanged(IChannelUser & pChatter);
	void chatterLost(IChannelUser & pChatter);
	void chatMessage(IChannelUser & pChatter, QString pMessage, SmileyList const & pSmilies);

protected:
	void setState(State pState);

	IChannel & mChannel;
	State    mState;
	QList<IChannelUser*> mChatters;
};

#endif // CORE_I_CHANNEL_CHAT_H_
