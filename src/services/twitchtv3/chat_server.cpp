#include "config.h"
#include "chat_server.h"
#include "chat_channel.h"
#include "channel.h"
#include "profile.h"

#include <QDateTime>
#include <QHostInfo>
#include <QTcpSocket>
#include <QTimer>

static const int MAX_PENDING = 4;

namespace twitchtv3
{

QStringRef ChatServer::RawMessage::username() const
{
	int lAt = source.indexOf('!');
	if (lAt == -1)
		return QStringRef();

	return source.leftRef(lAt);
}

ChatServer::ChatServer()
{
	mState = NONE;
	mSocket = new QTcpSocket();
	connect(mSocket, &QTcpSocket::stateChanged, this, &ChatServer::socketStateChanged);
	connect(mSocket, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error), this, &ChatServer::socketError);
	connect(mSocket, &QTcpSocket::readyRead, this, &ChatServer::socketReady);
	mPendingPoints = MAX_PENDING + 3;
	mPendingTimer = new QTimer();
	mPendingTimer->setInterval(2000);
	connect(mPendingTimer, &QTimer::timeout, this, &ChatServer::sendPing);
}

ChatServer::~ChatServer()
{
	mPendingMessages.clear();
	delete mPendingTimer;

	if (mSocket->isOpen())
	{
		mPendingPoints = 1;
		if (mState == CONNECTED)
		{
			connect(mSocket, &QTcpSocket::disconnected, mSocket, &QObject::deleteLater);
			sendRaw("QUIT");
		}
		else
		{
			mSocket->abort();
			mSocket->deleteLater();
		}
	}
	else
		mSocket->deleteLater();
}

bool ChatServer::isAnonymous() const
{
	return mAccount.isEmpty();
}

bool ChatServer::isConnected() const
{
	return mState == CONNECTED;
}

void ChatServer::setLogin(QString pAccount, QString pToken)
{
	Q_ASSERT(mState == NONE);

	mAccount = pAccount;
	mToken = pToken;
}

void ChatServer::connectToServer()
{
	if (mState != NONE)
		return;

	Q_ASSERT(!mSocket->isOpen());

	setState(CONNECTING);
	mSocket->connectToHost("irc.twitch.tv", 6667);
}

void ChatServer::disconnectFromServer()
{
	switch (mState)
	{
		case NONE:
		case DISCONNECTING:
			break;

		case CONNECTING:
			setState(DISCONNECTING);
			mSocket->abort();
			break;

		case AUTHENTICATING:
			setState(DISCONNECTING);
			break;

		case CONNECTED:
			setState(DISCONNECTING);
			sendRaw("QUIT :Bye");
			break;
	}
}

void ChatServer::setState(State pNewState)
{
	if (mState != pNewState)
	{
		mState = pNewState;
		emit stateChanged();
	}
}

void ChatServer::socketStateChanged(QAbstractSocket::SocketState pState)
{
	switch (pState)
	{
		case QAbstractSocket::UnconnectedState:
			setState(NONE);
			mSocket->close();
			break;

		case QAbstractSocket::ConnectedState:
			if (mState == DISCONNECTING)
			{
				sendRaw("QUIT :Connecting aborted");
			}
			else
			{
				setState(AUTHENTICATING);

				if (mAccount.isEmpty())
				{
					qsrand(QDateTime::currentDateTime().toTime_t());
					int lNum = qrand() % 1000000;

					QString lNickname = QString("justinfan%1").arg(lNum, 6, 10, QChar('0'));
					sendRaw(QString("USER guest - %2 :Anonymous user using Qlivestream\nNICK %1").arg(lNickname).arg(mSocket->peerName()));
				}
				else
				{
					sendRaw(QString("PASS oauth:%3\nUSER %1 - %2 :%1 using Qlivestream\nNICK %1").arg(mAccount).arg(mSocket->peerName()).arg(mToken));
				}
			}
			break;

		default:
			break;
	}
}

void ChatServer::socketError(QAbstractSocket::SocketError pError)
{
	switch (pError)
	{
		case QAbstractSocket::ConnectionRefusedError:
			mLastError = tr("Connection refused");
			return;

		case QAbstractSocket::RemoteHostClosedError:
			mLastError = tr("Remote host closed connection");
			return;

		case QAbstractSocket::HostNotFoundError:
			mLastError = tr("Host not found");
			return;

		default:
			break;
	}

	mLastError = QString("Socket error #%1").arg((int)pError);
}

void ChatServer::joinChannel(ChatChannel * pChannel)
{
	Q_ASSERT(pChannel != nullptr);

	if (mChannels.contains(pChannel))
		return;

	Q_ASSERT(pChannel->state() == ChatChannel::NONE);

	mChannels.append(pChannel);

	if (mState == CONNECTED)
	{
		sendRaw(QString("JOIN #%1").arg(pChannel->name()));
	}
	else
	{
		connectToServer();
	}

	pChannel->setState(ChatChannel::JOINING);
}

void ChatServer::leaveChannel(ChatChannel * pChannel, QString pMessage)
{
	Q_ASSERT(pChannel != nullptr);

	if (!mChannels.contains(pChannel))
		return;

	Q_ASSERT(pChannel->state() != ChatChannel::NONE);

	if (pChannel->state() == ChatChannel::JOINED)
	{
		if (pMessage.isEmpty())
			sendRaw(QString("PART #%1").arg(pChannel->name()));
		else
			sendRaw(QString("PART #%1 :%2").arg(pChannel->name()).arg(pMessage));
	}

	pChannel->setState(ChatChannel::LEAVING);
}

void ChatServer::removeChannel(ChatChannel * pChannel)
{
	Q_ASSERT(pChannel != nullptr);

	if (!mChannels.contains(pChannel))
		return;

	Q_ASSERT(pChannel->state() != ChatChannel::NONE);

	ChatChannel::State lState = pChannel->state();

	if (lState == ChatChannel::JOINED)
	{
		sendRaw(QString("PART #%1").arg(pChannel->name()));
		pChannel->setState(ChatChannel::LEAVING);
	}

	pChannel->setState(ChatChannel::NONE);
	mChannels.removeAll(pChannel);
}

void ChatServer::sendRaw(QString pMessage)
{
	QVector<QStringRef> lItems = pMessage.splitRef('\n', QString::SkipEmptyParts);

	QByteArrayList lNormalized;
	for (QStringRef const & lItem : lItems)
	{
		QByteArray lBytes = lItem.trimmed().toUtf8();

		if (lBytes.length() > 510)
		{
			qCritical() << "Chat message too long, message truncated! got" << lBytes.length() << "bytes =" << lItem;
			lBytes.truncate(510);
		}

		lBytes += "\r\n";
		lNormalized.append(lBytes);
	}

	QMutexLocker lLock(&mSocketMutex);

	int lSent = 0;
	while (lSent < lNormalized.length() && mPendingPoints > 0)
	{
		sendRawBytes(lNormalized[lSent]);
		--mPendingPoints;
		++lSent;
	}

	while (lSent < lNormalized.length())
	{
		mPendingMessages.append(lNormalized[lSent]);
		++lSent;
	}

	if (!mPendingTimer->isActive())
		mPendingTimer->start();
}

void ChatServer::socketReady()
{
	QString lData;
	lData.swap(mSocketReceiveBuffer);
	lData.append(mSocket->readAll());

	QVector<QStringRef> lDataSplit = lData.splitRef('\n');
	if (lDataSplit.isEmpty())
		return;

	mSocketReceiveBuffer = lDataSplit.takeLast().trimmed().toString();

	for (QStringRef const & lSub : lDataSplit)
	{
		QStringRef lLine = lSub.trimmed();
		if (lLine.length() == 0)
			continue;

		qDebug() << "[TWITCHTV3 CHAT] <<" << lLine;

		RawMessage lMessage;

		// Parse tags
		if (lLine.startsWith('@'))
		{
			lLine = lLine.mid(1);

			QStringRef lData = lLine.left(lLine.indexOf(' '));
			lLine = lLine.mid(lData.length() + 1).trimmed();

			while (!lData.isEmpty())
			{
				QStringRef lParam = lData.left(lData.indexOf(';'));
				lData = lData.mid(lParam.length() + 1);

				QString lKey;
				QString lValue;
				int lEqualsLoc = lParam.indexOf('=');
				if (lEqualsLoc == -1)
				{
					lKey = lParam.toString();
				}
				else
				{
					lKey = lParam.left(lEqualsLoc).toString();
					lValue = lParam.mid(lEqualsLoc+1).toString();

					// Unescaping as per http://ircv3.net/specs/core/message-tags-3.2.html

					lValue.replace("\\\\", QChar(0));
					lValue.replace("\\:", ";");
					lValue.replace("\\s", " ");
					lValue.replace("\\r", "\r");
					lValue.replace("\\n", "\n");
					lValue.replace(QChar(0), QChar('\\'));
				}

				lMessage.tags.insert(lKey, lValue);
			}
		}

		// Parse source
		if (lLine.startsWith(':'))
		{
			QStringRef lData = lLine.left(lLine.indexOf(' '));
			lLine = lLine.mid(lData.length() + 1).trimmed();

			lMessage.source = lData.mid(1).toString();
		}

		if (!lLine.isEmpty())
		{
			QStringRef lData = lLine.left(lLine.indexOf(' '));
			lLine = lLine.mid(lData.length() + 1).trimmed();

			lMessage.command = lData.toString();
		}

		while (!lLine.isEmpty())
		{
			if (lLine.startsWith(':'))
			{
				lMessage.parameters.append(lLine.mid(1).toString());
				break;
			}
			else
			{
				QStringRef lData = lLine.left(lLine.indexOf(' '));
				lLine = lLine.mid(lData.length() + 1).trimmed();

				lMessage.parameters.append(lData.toString());
			}
		}

		handleMessage(lMessage);
	}
}

void ChatServer::sendPing()
{
	QMutexLocker lLock(&mSocketMutex);

	if (mPendingMessages.empty())
	{
		++mPendingPoints;
		if (mPendingPoints >= MAX_PENDING)
			mPendingTimer->stop();
	}
	else
	{
		QByteArray lMessage = mPendingMessages.takeFirst();
		sendRawBytes(lMessage);
	}
}

void ChatServer::sendRawBytes(QByteArray const & pMessage)
{
	qDebug() << "[TWITCHTV3 CHAT] >>" << QString::fromUtf8(pMessage).trimmed();
	mSocket->write(pMessage);
}

void ChatServer::handleMessage(RawMessage const & pMessage)
{
	//qDebug() << "MESSAGE TAGS=" << pMessage.tags << "SOURCE=" << pMessage.source << "COMMAND=" << pMessage.command << "PARAMETERS=" << pMessage.parameters;

	if (pMessage.command == QStringLiteral("PING"))
	{
		if (mChannels.isEmpty())
		{
			setState(DISCONNECTING);
			sendRaw(QString("QUIT :Bye"));
		}
		else
			sendRaw(QString("PONG :%1").arg(pMessage.parameters.join(' ')));

		return;
	}

	else if (pMessage.command == QStringLiteral("001"))
	{
		mNickname = pMessage.parameters.value(0);
		setState(CONNECTED);

		sendRaw("CAP REQ :twitch.tv/tags");
		sendRaw("CAP REQ :twitch.tv/membership");
		sendRaw("CAP REQ :twitch.tv/commands");

		for (ChatChannel * lChannel : mChannels)
			sendRaw(QString("JOIN #%1").arg(lChannel->name()));
	}

	else if (pMessage.command == QStringLiteral("JOIN"))
	{
		bool lSelf = (pMessage.username() == mNickname);

		ChatChannel * lChannel = findChannel(pMessage.parameters.value(0));
		if (lChannel != nullptr)
		{
			if (lSelf)
			{
				if (lChannel->state() == ChatChannel::LEAVING)
					sendRaw(QString("PART #%1").arg(lChannel->name()));
				else
				{
					lChannel->setState(ChatChannel::JOINED);
					lChannel->onJoin(pMessage.source);
				}
			}
			else
				lChannel->onJoin(pMessage.source);
		}
	}

	else if (pMessage.command == QStringLiteral("PART"))
	{
		bool lSelf = (pMessage.username() == mNickname);

		ChatChannel * lChannel = findChannel(pMessage.parameters.value(0));
		if (lChannel != nullptr)
		{
			lChannel->onPart(pMessage.source, pMessage.parameters.value(1));
			if (lSelf)
			{
				mChannels.removeAll(lChannel);
				lChannel->setState(ChatChannel::NONE);
			}
		}
	}

	else if (pMessage.command == QStringLiteral("PRIVMSG"))
	{
		QString lTarget = pMessage.parameters.value(0);
		if (lTarget.startsWith('#'))
		{
			ChatChannel * lChannel = findChannel(pMessage.parameters.value(0));
			if (lChannel != nullptr)
				lChannel->onPrivmsg(pMessage.source, pMessage.tags, pMessage.parameters.value(1));
		}
	}

	else if (pMessage.command == QStringLiteral("USERSTATE"))
	{
		ChatChannel * lChannel = findChannel(pMessage.parameters.value(0));
		if (lChannel != nullptr)
			lChannel->onUserstate(pMessage.tags);
	}

	else if (pMessage.command == QStringLiteral("ROOMSTATE"))
	{
		ChatChannel * lChannel = findChannel(pMessage.parameters.value(0));
		if (lChannel != nullptr)
			lChannel->onRoomstate(pMessage.tags);
	}

	emit newMessage(pMessage);
}

ChatChannel * ChatServer::findChannel(QString pName) const
{
	QString lName = pName.mid(1).toLower();

	auto i = std::find_if(mChannels.begin(), mChannels.end(),
					[&lName] (ChatChannel * lChannel) -> bool { return lChannel->channel().name().toLower() == lName; });

	if (i != mChannels.end())
		return *i;

	return nullptr;
}

} // namespace twitchtv3
