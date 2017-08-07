#include "channel_chat.h"

#include <qglobal.h>
#include <QChar>
#include <QList>
#include <QStringList>
#include "channel.h"

#include "profile.h"
#include "user.h"

namespace twitchtv
{

ChannelChat::ChannelChat(Channel & pChannel, ChatServer::Ptr pServer) : IChannelChat(pChannel), mServer(pServer)
{
	mRoomR9K = false;
	mRoomSlow = false;
	mRoomSubsOnly = false;

	mSelf = static_cast<ChannelUser*>(profile().self().getChannelUser(mChannel, true));
	Q_ASSERT(mSelf != nullptr);
}

ChannelChat::~ChannelChat()
{
	mServer->removeChannel(*this);
}

Profile & ChannelChat::profile() const
{
	return channel().owner().profile();
}

User & ChannelChat::owner() const
{
	return channel().owner();
}

Channel & ChannelChat::channel() const
{
	return static_cast<Channel&>(mChannel);
}

QString ChannelChat::name() const
{
	return channel().owner().name();
}

void ChannelChat::connectToChat()
{
	mServer->joinChannel(*this);
}

void ChannelChat::disconnectFromChat(QString pMessage)
{
	mServer->leaveChannel(*this, pMessage);
}

void ChannelChat::sendMessage(QString pMessage)
{
	if (mServer->isAnonymous())
		return;

	QString lMessage = QString("PRIVMSG #%1 :%2").arg(name()).arg(pMessage);
	mServer->sendRaw(lMessage);

	SmileyList lSmilies;
	emit chatMessage(*mSelf, pMessage, lSmilies);
}

ChannelUser * ChannelChat::findChatter(QStringRef pUsername, bool pCreate) const
{
	if (pUsername.isEmpty())
		return nullptr;

	IUser * lUser = profile().getUserFor(pUsername.toString(), pCreate);
	if (lUser == nullptr)
		return nullptr;

	return static_cast<ChannelUser*>(lUser->getChannelUser(mChannel, true));
}

void ChannelChat::onUserstate(ChatServer::Tags const & pTags)
{
	mSelf->updateFromTags(pTags);

	QString lSets = pTags.value("emote-sets");
	QStringList lSetList = lSets.split(',');
	mUserEmoteSets.clear();
	mUserEmoteSets.reserve(lSetList.size());

	for (QString const & lSet : lSetList)
	{
		bool lOK;
		int lValue = lSet.toInt(&lOK, 10);
		if (lOK && !mUserEmoteSets.contains(lValue))
			mUserEmoteSets.append(lValue);
	}
}

void ChannelChat::onRoomstate(ChatServer::Tags const & pTags)
{
	mRoomLanguage = pTags.value("broadcaster-lang");
	mRoomR9K = pTags.value("r9k") == "1";
	mRoomSlow = pTags.value("slow") == "1";
	mRoomSubsOnly = pTags.value("subs-only") == "1";
}

void ChannelChat::onJoin(QString pSource)
{
	QStringRef lUsername = usernameFrom(pSource);
	if (lUsername.isEmpty())
		return;

	ChannelUser * lChatter = findChatter(lUsername);
	if (mChatters.contains(lChatter))
		return;

	mChatters.append(lChatter);
	emit chatterNew(*lChatter);
}

void ChannelChat::onPart(QString pSource, QString pMessage)
{
	QStringRef lUsername = usernameFrom(pSource);
	if (lUsername.isEmpty())
		return;

	ChannelUser * lChatter = findChatter(lUsername, false);
	if (lChatter == nullptr || !mChatters.contains(lChatter))
		return;

	mChatters.removeAll(lChatter);
	emit chatterLost(*lChatter);
}

void ChannelChat::onMode(QString pTarget, bool pAdd, ChannelUser::Flag pFlag)
{
	ChannelUser * lChatter = findChatter(pTarget.midRef(0));
	lChatter->updateFlag(pFlag, pAdd);
}

void ChannelChat::onPrivmsg(QString pSource, ChatServer::Tags const & pTags, QString pMessage)
{
	QStringRef lUsername = usernameFrom(pSource);
	if (lUsername.isEmpty())
		return;

	ChannelUser * lChatter = findChatter(lUsername);
	lChatter->updateFromTags(pTags);

	if (!mChatters.contains(lChatter))
	{
		mChatters.append(lChatter);
		emit chatterNew(*lChatter);
	}

	SmileyList lSmilies;
	emit chatMessage(*lChatter, pMessage, lSmilies);
}

QStringRef ChannelChat::usernameFrom(QString pSource)
{
	int lAt = pSource.indexOf('!');
	if (lAt == -1)
		return pSource.midRef(0);

	return pSource.leftRef(lAt);
}

} // namespace twitchtv
