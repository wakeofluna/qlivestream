#ifndef CORE_I_CHANNEL_USER_H_
#define CORE_I_CHANNEL_USER_H_

#include <qglobal.h>
#include <QObject>
#include <QString>
#include "class_bitset.h"

#include "../config.h"

class IUser;
class IChannel;

class COREDLL IChannelUser : public QObject
{
Q_OBJECT

public:
	union Color
	{
		inline Q_DECL_CONSTEXPR Color() : v(0) {}
		inline Q_DECL_CONSTEXPR Color(quint8 _r, quint8 _g, quint8 _b) : b(_b), g(_g), r(_r), a(255) {}
		inline Q_DECL_CONSTEXPR bool isValid() const { return a != 0; }
		inline Q_DECL_CONSTEXPR bool operator== (Color const& c) const { return v == c.v; }
		inline Q_DECL_CONSTEXPR bool operator!= (Color const& c) const { return v != c.v; }

		struct { quint8 b, g, r, a; };
		quint32 v;
	};

	struct Flag
	{
		enum Value
		{
			FOLLOWER,
			SUBSCRIBER,
			MODERATOR,
			INVALID
		};
		// cppcheck-suppress noExplicitConstructor
		ENUMSTRUCT(Flag, Value, INVALID);
	};
	typedef ClassBitset<Flag> Flags;

public:
	IChannelUser(IChannel & pChannel, IUser & pUser);
	virtual ~IChannelUser();

	inline IChannel & channel() const { return mChannel; }
	inline IUser & user() const { return mUser; }
	inline QString displayName() const { return mDisplayName; }
	inline Color color() const { return mColor; }

	bool isSelf() const;
	bool isOwner() const;
	inline bool isFollower() const { return mFlags.test(Flag::FOLLOWER); }
	inline bool isSubscriber() const { return mFlags.test(Flag::SUBSCRIBER); }
	inline bool isModerator() const { return mFlags.test(Flag::MODERATOR); }

	inline bool operator< (IChannelUser const& pOther) const { return mDisplayName < pOther.mDisplayName; }
	static bool sortStatus(IChannelUser const& lhs, IChannelUser const& rhs);

signals:
	void infoUpdated();

private slots:
	void onInfoUpdated();

protected:
	IChannel & mChannel;
	IUser    & mUser;
	QString  mDisplayName;
	Color    mColor;
	Flags    mFlags;
};

#endif // CORE_I_CHANNEL_USER_H_
