#ifndef CORE_CATEGORY_OBJECT_H_
#define CORE_CATEGORY_OBJECT_H_

#include "config.h"
#include <QObject>
#include <QString>
#include <QUrl>
#include <QVariant>

class Profile;

class COREDLL CategoryObject : public QObject
{
Q_OBJECT

public:
	explicit CategoryObject(Profile & pProfile);
	virtual ~CategoryObject();

	inline Profile const & profile() const { return mProfile; }
	inline Profile & profile() { return mProfile; }

	inline QVariant id() const { return mId; }
	inline QString name() const { return mName; }
	inline QUrl logoUrl() const { return mLogoUrl; }
	inline int numChannels() const { return mNumChannels; }
	inline int numViewers() const { return mNumViewers; }
	inline bool followed() const { return mFollowed; }

	virtual QString logoCacheString() const;
	static QString logoCacheString(QString pTag);

	bool isValid() const;

	void clear();
	bool updateFrom(CategoryObject const & pOther);
	bool operator< (CategoryObject const & pOther) const;

signals:
	void statsChanged();

protected:
	void setStats(int pChannels, int pViewers);

	Profile & mProfile;
	QVariant mId;
	QString  mName;
	QUrl     mLogoUrl;
	int      mNumChannels;
	int      mNumViewers;
	bool     mFollowed;
};

#endif // CORE_CATEGORY_OBJECT_H_
