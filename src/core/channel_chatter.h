#ifndef CORE_CHANNEL_CHATTER_H_
#define CORE_CHANNEL_CHATTER_H_

#include "config.h"
#include <QString>

class ChannelChat;
class COREDLL ChannelChatter
{
public:
	ChannelChatter(ChannelChat & pChannel, QString pName);
	virtual ~ChannelChatter();

	inline ChannelChat & channel() const { return mChannel; }
	inline QString name() const { return mName; }

	inline bool isSelf() const { return mIsSelf; }
	inline bool isFollower() const { return mIsFollower; }
	inline bool isSubscriber() const { return mIsSubscriber; }
	inline bool isModerator() const { return mIsModerator; }
	inline bool isOwner() const { return mIsOwner; }

	inline bool operator== (ChannelChatter const& pOther) const { return mName == pOther.mName; }
	inline bool operator!= (ChannelChatter const& pOther) const { return mName != pOther.mName; }
	inline bool operator<  (ChannelChatter const& pOther) const { return mName < pOther.mName; }
	static bool sortStatus(ChannelChatter const& lhs, ChannelChatter const& rhs);

private:
	ChannelChat & mChannel;
	QString mName;
	bool mIsSelf;
	bool mIsFollower;
	bool mIsSubscriber;
	bool mIsModerator;
	bool mIsOwner;
};

#endif // CORE_CHANNEL_CHATTER_H_
