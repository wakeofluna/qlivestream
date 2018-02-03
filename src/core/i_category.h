#ifndef CORE_I_CATEGORY_H_
#define CORE_I_CATEGORY_H_

#include <QList>
#include <QString>
#include <QUrl>
#include <QVariant>
#include "class_bitset.h"
#include "i_channel.h"

#include "../config.h"

class IProfile;

class COREDLL ICategory : public QObject
{
Q_OBJECT

public:
	enum UrlType
	{
		URL_LOGO,
		URL_CATEGORY
	};

	struct Flag
	{
		enum Value
		{
			FOLLOWED,
			INVALID
		};
		// cppcheck-suppress noExplicitConstructor
		ENUMSTRUCT(Flag, Value, INVALID);
	};
	typedef ClassBitset<Flag> Flags;

public:
	explicit ICategory(IProfile & pProfile);
	virtual ~ICategory();

	inline IProfile const & profile() const { return mProfile; }
	inline IProfile & profile() { return mProfile; }

	inline QVariant id() const { return mId; }
	inline QString name() const { return mName; }
	inline int numChannels() const { return mNumChannels; }
	inline int numViewers() const { return mNumViewers; }
	inline bool isFollowed() const { return mFlags.test(Flag::FOLLOWED); }

	inline QList<IChannel *> channels() const { return mChannels; }

	virtual QString logoCacheString() const;
	static QString logoCacheString(QString pTag);

	bool operator< (ICategory const & pOther) const;

	virtual QUrl categoryUrl(UrlType pType) const = 0;
	virtual void setFollowed(bool pFollow) = 0;

public slots:
	void follow();
	void unfollow();
	void resetChannels();
	virtual void rollupChannels() = 0;

signals:
	void infoUpdated();
	void channelsUpdated();

protected:
	IProfile & mProfile;
	QVariant mId;
	QString  mName;
	Flags    mFlags;
	int      mNumChannels;
	int      mNumViewers;

	bool mCanRollupChannels;
	QList<IChannel *> mChannels;
};

#endif // CORE_I_CATEGORY_H_
