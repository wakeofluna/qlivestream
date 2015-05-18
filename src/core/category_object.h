#ifndef CORE_CATEGORY_OBJECT_H_
#define CORE_CATEGORY_OBJECT_H_

#include <QString>
#include <QUrl>

class CategoryObject
{
public:
	CategoryObject();
	virtual ~CategoryObject();

	inline QString name() const { return mName; }
	inline QUrl logoUrl() const { return mLogoUrl; }
	inline int numChannels() const { return mNumChannels; }
	inline int numViewers() const { return mNumViewers; }
	inline bool followed() const { return mFollowed; }

	virtual QString logoCacheString() const;

protected:
	void setStats(int pChannels, int pViewers);

	QString mName;
	QUrl    mLogoUrl;
	int     mNumChannels;
	int     mNumViewers;
	bool    mFollowed;
};

#endif // CORE_CATEGORY_OBJECT_H_
