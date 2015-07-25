#ifndef CORE_CHANNEL_CHATTER_H_
#define CORE_CHANNEL_CHATTER_H_

#include "config.h"
#include "core/class_bitset.h"
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

	struct Flag
	{
		enum Value
		{
			SELF,
			OWNER,
			FOLLOWER,
			SUBSCRIBER,
			VETERAN,
			MODERATOR,
			INVALID
		};

		typedef Value enum_type;
		static const enum_type max = INVALID;

		inline Flag() : mValue(INVALID) {}
		inline Flag(Value pValue) : mValue(pValue) {}
		inline operator Value() const { return mValue; }

		Value mValue;
	};
	typedef ClassBitset<Flag> Flags;

public:
	ChannelChatter(ChannelChat & pChannel, QString pName);
	virtual ~ChannelChatter();

	inline ChannelChat & channel() const { return mChannel; }
	inline QString name() const { return mName; }
	inline QString displayName() const { return !mDisplayName.isEmpty() ? mDisplayName : mName; }
	inline Color color() const { return mColor; }

	inline bool isSelf() const { return mFlags.test(Flag::SELF); }
	inline bool isOwner() const { return mFlags.test(Flag::OWNER); }
	inline bool isFollower() const { return mFlags.test(Flag::FOLLOWER); }
	inline bool isSubscriber() const { return mFlags.test(Flag::SUBSCRIBER); }
	inline bool isVeteran() const { return mFlags.test(Flag::VETERAN); }
	inline bool isModerator() const { return mFlags.test(Flag::MODERATOR); }

	inline bool operator== (ChannelChatter const& pOther) const { return mName == pOther.mName; }
	inline bool operator!= (ChannelChatter const& pOther) const { return mName != pOther.mName; }
	inline bool operator<  (ChannelChatter const& pOther) const { return mName < pOther.mName; }
	static bool sortStatus(ChannelChatter const& lhs, ChannelChatter const& rhs);

protected:
	bool setDisplayName(QString pName);
	bool setFlag(Flag pFlag, bool pEnabled);
	inline bool addFlag(Flag pFlag) { return setFlag(pFlag, true); }
	inline bool removeFlag(Flag pFlag) { return setFlag(pFlag, false); }

	ChannelChat & mChannel;
	QString mName;
	QString mDisplayName;
	Color mColor;
	Flags mFlags;
};

#endif // CORE_CHANNEL_CHATTER_H_
