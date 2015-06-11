#include "chat_server.h"

#include "config.h"
#include <QTcpSocket>

namespace twitchtv3
{

ChatServer::ChatServer(QObject * parent) : QObject(parent)
{
	mSocket = new QTcpSocket(this);
}

ChatServer::~ChatServer()
{
	delete mSocket;
}


} // namespace twitchtv3
