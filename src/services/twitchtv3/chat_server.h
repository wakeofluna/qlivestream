#ifndef SERVICES_TWITCHTV3_CHAT_SERVER_H_
#define SERVICES_TWITCHTV3_CHAT_SERVER_H_

#include <QObject>
class QTcpSocket;

namespace twitchtv3
{

class ChatServer : public QObject
{
Q_OBJECT

public:
	explicit ChatServer(QObject * parent = 0);
	virtual ~ChatServer();

	bool isConnected();

public slots:
	void connectToServer();
	void disconnectFromServer();

signals:
	void connected();
	void disconnected();

private:
	QTcpSocket * mSocket;
};

} // namespace twitchtv3

#endif // SERVICES_TWITCHTV3_CHAT_SERVER_H_
