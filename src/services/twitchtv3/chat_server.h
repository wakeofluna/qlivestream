#ifndef SERVICES_TWITCHTV3_CHAT_SERVER_H_
#define SERVICES_TWITCHTV3_CHAT_SERVER_H_

#include <QByteArrayList>
#include <QMap>
#include <QMutex>
#include <QObject>
#include <QSharedData>
#include <QString>
#include <QTcpSocket>
class QTimer;

namespace twitchtv3
{

class Channel;
class ChannelChat;
class Profile;

class ChatServer : public QObject, public QSharedData
{
Q_OBJECT

public:
	typedef QExplicitlySharedDataPointer<ChatServer> Ptr;
	typedef QMap<QString,QString> Tags;

	enum State
	{
		NONE,
		CONNECTING,
		AUTHENTICATING,
		CONNECTED,
		DISCONNECTING
	};

	struct RawMessage
	{
		Tags tags;
		QString source;
		QString command;
		QStringList parameters;

		QStringRef username() const;
	};

public:
	explicit ChatServer(Profile & pProfile);
	virtual ~ChatServer();

	inline Profile & profile() const { return mProfile; }
	inline QString nickname() const { return mNickname; }
	inline QString lastError() const { return mLastError; }

	QString account() const;
	bool isAnonymous() const;
	bool isConnected() const;

	void joinChannel(ChannelChat & pChannel);
	void leaveChannel(ChannelChat & pChannel, QString pMessage);
	void removeChannel(ChannelChat & pChannel);
	void sendRaw(QString pMessage);

public slots:
	void connectToServer();
	void disconnectFromServer();

signals:
	void stateChanged();
	void newMessage(RawMessage const & pMessage);

protected:
	void setState(State pNewState);

private slots:
	void socketStateChanged(QAbstractSocket::SocketState pState);
	void socketError(QAbstractSocket::SocketError pError);
	void socketReady();

private:
	void sendPing();
	void sendRawBytes(QByteArray const & pMessage);
	void handleMessage(RawMessage const & pMessage);
	ChannelChat * findChannel(QString pName) const;

	Profile & mProfile;
	QString mToken;
	QString mNickname;
	QTcpSocket * mSocket;
	QString mLastError;
	State mState;

	QMutex mSocketMutex;
	QString mSocketReceiveBuffer;
	QTimer * mPendingTimer;
	int mPendingPoints;
	QByteArrayList mPendingMessages;

	QList<ChannelChat*> mChannels;
};

} // namespace twitchtv3

#endif // SERVICES_TWITCHTV3_CHAT_SERVER_H_
