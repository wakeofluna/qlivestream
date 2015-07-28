#ifndef CORE_I_USER_H_
#define CORE_I_USER_H_

#include <memory>
#include <QMap>
#include <QString>
#include <QVariant>
#include "class_bitset.h"
#include "i_channel.h"
#include "i_channel_user.h"

#include "../config.h"

class IChannel;
class IProfile;

class COREDLL IUser : public QObject
{
Q_OBJECT

public:
	struct Flag
	{
		enum Value
		{
			VETERAN,
			STAFF,
			FRIEND,
			INVALID
		};
		ENUMSTRUCT(Flag, Value, INVALID);
	};
	typedef ClassBitset<Flag> Flags;

public:
	explicit IUser(IProfile & pProfile);
	virtual ~IUser();

	inline IProfile & profile() const { return mProfile; }
	inline QVariant id() const { return mId; }
	inline QString name() const { return mName; }
	inline bool isVeteran() const { return mFlags.test(Flag::VETERAN); }
	inline bool isFriend() const { return mFlags.test(Flag::FRIEND); }

	bool isSelf() const;
	IChannel * channel(bool pCreate);
	IChannelUser * getChannelUser(IChannel & pChannel, bool pCreate);

	inline bool operator== (IUser const & pOther) const { return mId == pOther.mId; }

public slots:
	virtual void befriend() = 0;
	virtual void unfriend() = 0;

signals:
	void infoUpdated();

protected:
	virtual IChannel * newChannel() = 0;
	virtual IChannelUser * newChannelUser(IChannel & pChannel) = 0;

	IProfile & mProfile;
	QVariant mId;
	QString  mName;
	Flags    mFlags;

	std::unique_ptr<IChannel> mChannel;
	QMap<IChannel *, IChannelUser *> mChannelUsers;
};

#endif // CORE_I_USER_H_
