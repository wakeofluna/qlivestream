#ifndef CORE_CHANNEL_CHAT_H_
#define CORE_CHANNEL_CHAT_H_

#include "config.h"
#include <QObject>
#include <QString>

class ChannelChatter;
class ChannelObject;
template <typename T> class QList;
template <typename T> class QVector;

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

	struct SmileyInfo
	{
		int from;
		int to;
		QString id;
	};
	typedef QVector<SmileyInfo> SmileyList;

public:
	explicit ChannelChat(ChannelObject & pChannel);
	~ChannelChat();

	QString name() const;
	inline ChannelObject & channel() const { return mChannel; }
	inline State state() const { return mState; }

public slots:
	virtual void connectToChat() = 0;
	virtual void disconnectFromChat(QString pMessage = QString()) = 0;
	virtual void sendMessage(QString pMessage) = 0;

signals:
	void chatStateChanged();
	void chatError(QString pMessage);

	void chatterNew(ChannelChatter & pChatter);
	void chatterChanged(ChannelChatter & pChatter);
	void chatterLost(ChannelChatter & pChatter);
	void chatMessage(ChannelChatter & pChatter, QString pMessage, SmileyList const & pSmilies);

protected:
	void setState(State pNewState);

	ChannelObject & mChannel;
	State mState;
	ChannelChatter * mSelf;
	QList<ChannelChatter*> mChatters;
	QList<ChannelChatter*> mChattersLeft;
};

#endif // CORE_CHANNEL_CHAT_H_
