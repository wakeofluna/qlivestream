#ifndef CORE_CHANNEL_CHATTER_H_
#define CORE_CHANNEL_CHATTER_H_

#include "config.h"
#include <QString>

class ChannelChat;
class COREDLL ChannelChatter
{
public:
	union Color
	{
		inline Color() : v(0) {}
		inline bool isValid() const { return a != 0; }
		inline bool operator== (Color const& c) const { return v == c.v; }
		inline bool operator!= (Color const& c) const { return v != c.v; }

		struct { quint8 b, g, r, a; };
		quint32 v;
	};

public:
	ChannelChatter(ChannelChat & pChannel, QString pName);
	virtual ~ChannelChatter();

	inline ChannelChat & channel() const { return mChannel; }
	inline QString name() const { return mName; }
	inline QString displayName() const { return !mDisplayName.isEmpty() ? mDisplayName : mName; }
	inline Color color() const { return mColor; }

	inline bool isSelf() const { return mIsSelf; }
	inline bool isFollower() const { return mIsFollower; }
	inline bool isSubscriber() const { return mIsSubscriber; }
	inline bool isVeteran() const { return mIsVeteran; }
	inline bool isModerator() const { return mIsModerator; }
	inline bool isOwner() const { return mIsOwner; }

	inline bool operator== (ChannelChatter const& pOther) const { return mName == pOther.mName; }
	inline bool operator!= (ChannelChatter const& pOther) const { return mName != pOther.mName; }
	inline bool operator<  (ChannelChatter const& pOther) const { return mName < pOther.mName; }
	static bool sortStatus(ChannelChatter const& lhs, ChannelChatter const& rhs);

protected:
	ChannelChat & mChannel;
	QString mName;
	QString mDisplayName;
	Color mColor;
	bool mIsSelf;
	bool mIsFollower;
	bool mIsSubscriber;
	bool mIsVeteran;
	bool mIsModerator;
	bool mIsOwner;
};

#endif // CORE_CHANNEL_CHATTER_H_
