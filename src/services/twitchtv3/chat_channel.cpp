#include "config.h"
#include "chat_channel.h"
#include "channel.h"
#include "chat_server.h"
#include "channel_chatter.h"

namespace twitchtv3
{

ChatChannel::ChatChannel(Channel & pChannel, ChatServer::Ptr pServer) : ChannelChat(pChannel), mServer(pServer)
{
	mUserSub = false;
	mUserTurbo = false;
	mRoomR9K = false;
	mRoomSlow = false;
	mRoomSubsOnly = false;
}

ChatChannel::~ChatChannel()
{
	mServer->removeChannel(this);
}

Channel & ChatChannel::channel() const
{
	return static_cast<Channel&>(mChannel);
}

void ChatChannel::connectToChat()
{
	mServer->joinChannel(this);
}

void ChatChannel::disconnectFromChat(QString pMessage)
{
	mServer->leaveChannel(this, pMessage);
}

void ChatChannel::sendMessage(QString pMessage)
{
	QString lMessage = QString("PRIVMSG #%1 :%2").arg(name()).arg(pMessage);
	mServer->sendRaw(lMessage);

	Q_ASSERT(mSelf != nullptr);

	SmileyList lSmilies;
	emit chatMessage(*mSelf, pMessage, lSmilies);
}

ChannelChatter * ChatChannel::findChatter(QStringRef pUsername, bool pOld) const
{
	for (::ChannelChatter * lChatter : (pOld ? mChattersLeft : mChatters))
		if (lChatter->name().compare(pUsername, Qt::CaseInsensitive) == 0)
			return static_cast<ChannelChatter*>(lChatter);

	return nullptr;
}

void ChatChannel::onUserstate(ChatServer::Tags const & pTags)
{
	mUserColor = pTags.value("color");
	mUserDisplayName = pTags.value("display-name");
	mUserEmoteSets.clear();
	mUserSub = pTags.value("subscriber") == "1";
	mUserTurbo = pTags.value("turbo") == "1";
	mUserType = pTags.value("user-type");

	QString lSets = pTags.value("emote-sets");
	QStringList lSetList = lSets.split(',');
	mUserEmoteSets.reserve(lSetList.size());

	for (QString const & lSet : lSetList)
	{
		bool lOK;
		int lValue = lSet.toInt(&lOK, 10);
		if (lOK && !mUserEmoteSets.contains(lValue))
			mUserEmoteSets.append(lValue);
	}
}

void ChatChannel::onRoomstate(ChatServer::Tags const & pTags)
{
	mRoomLanguage = pTags.value("broadcaster-lang");
	mRoomR9K = pTags.value("r9k") == "1";
	mRoomSlow = pTags.value("slow") == "1";
	mRoomSubsOnly = pTags.value("subs-only") == "1";
}

void ChatChannel::onJoin(QString pSource, ChatServer::Tags const * pTags)
{
	QStringRef lUsername = usernameFrom(pSource);
	if (lUsername.isEmpty())
		return;

	ChannelChatter * lChatter = findChatter(lUsername);
	if (lChatter != nullptr)
		return;

	lChatter = findChatter(lUsername, true);
	if (lChatter != nullptr)
		mChattersLeft.removeAll(lChatter);
	else
	{
		lChatter = new ChannelChatter(*this, lUsername.toString());
		lChatter->setFlag(ChannelChatter::Flag::SELF, lUsername == mServer->nickname());
		lChatter->setFlag(ChannelChatter::Flag::OWNER, lUsername == mChannel.name());

		if (lChatter->isSelf())
			mSelf = lChatter;
	}

	if (pTags != nullptr)
		lChatter->updateFromTags(*pTags);

	mChatters.append(lChatter);

	emit chatterNew(*lChatter);
}

void ChatChannel::onPart(QString pSource, QString pMessage)
{
	QStringRef lUsername = usernameFrom(pSource);
	if (lUsername.isEmpty())
		return;

	ChannelChatter * lChatter = findChatter(lUsername);
	if (lChatter == nullptr)
		return;

	mChatters.removeAll(lChatter);
	mChattersLeft.append(lChatter);

	emit chatterLost(*lChatter);
}

void ChatChannel::onMode(QString pTarget, bool pAdd, ChannelChatter::Flag pFlag)
{
	ChannelChatter * lChatter = findChatter(pTarget.midRef(0));
	if (lChatter == nullptr)
		return;

	if (lChatter->setFlag(pFlag, pAdd))
		emit chatterChanged(*lChatter);
}

void ChatChannel::onPrivmsg(QString pSource, ChatServer::Tags const & pTags, QString pMessage)
{
	QStringRef lUsername = usernameFrom(pSource);
	if (lUsername.isEmpty())
		return;

	ChannelChatter * lChatter = findChatter(lUsername);
	if (lChatter == nullptr)
	{
		onJoin(pSource, &pTags);
		lChatter = findChatter(lUsername);
		if (lChatter == nullptr)
			return;
	}
	else if (lChatter->updateFromTags(pTags))
		emit chatterChanged(*lChatter);

	SmileyList lSmilies;
	emit chatMessage(*lChatter, pMessage, lSmilies);
}

QStringRef ChatChannel::usernameFrom(QString pSource)
{
	int lAt = pSource.indexOf('!');
	if (lAt == -1)
		return QStringRef();

	return pSource.leftRef(lAt);
}

} // namespace twitchtv3
