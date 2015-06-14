#ifndef CORE_CHANNEL_CHAT_H_
#define CORE_CHANNEL_CHAT_H_

#include "config.h"
#include <QObject>
#include <QString>

class ChannelChatter;
template <typename T> class QList;

class COREDLL ChannelChat : public QObject
{
Q_OBJECT

public:
	explicit ChannelChat(QObject * parent);
	~ChannelChat();

	inline QString name() const { return mName; }
	inline bool isConnected() const { return mIsConnected; }

public slots:
	virtual void connectToChat() = 0;
	virtual void disconnectFromChat() = 0;

signals:
	void chatConnected();
	void chatError(QString pMessage);
	void chatDisconnected();

	void newChatter(ChannelChatter & pChatter);
	void newMessage(ChannelChatter & pChatter, QString pMessage);
	void lostChatter(ChannelChatter & pChatter);

private:
	QString mName;
	bool mIsConnected;
	QList<ChannelChatter*> mChatters;
};

#endif // CORE_CHANNEL_CHAT_H_
