#ifndef CORE_CHANNEL_CHAT_H_
#define CORE_CHANNEL_CHAT_H_

#include "config.h"
#include <QObject>
#include <QString>

class ChannelChatter;
class ChannelObject;
template <typename T> class QList;

class COREDLL ChannelChat : public QObject
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

public:
	explicit ChannelChat(ChannelObject & pChannel);
	~ChannelChat();

	QString name() const;
	inline ChannelObject & channel() const { return mChannel; }

	virtual inline State state() const { return mState; }

public slots:
	virtual void connectToChat() = 0;
	virtual void disconnectFromChat(QString pMessage = QString()) = 0;
	virtual void sendMessage(QString pMessage) = 0;

signals:
	void chatStateChanged();
	void chatError(QString pMessage);

	void newChatter(ChannelChatter & pChatter);
	void newMessage(ChannelChatter & pChatter, QString pMessage);
	void lostChatter(ChannelChatter & pChatter);

protected:
	void setState(State pNewState);

	ChannelObject & mChannel;
	State mState;
	QList<ChannelChatter*> mChatters;
};

#endif // CORE_CHANNEL_CHAT_H_
