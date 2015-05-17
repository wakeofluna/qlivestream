#ifndef CORE_CATEGORY_OBJECT_H_
#define CORE_CATEGORY_OBJECT_H_

#include <QObject>
#include <QString>
#include <QUrl>

class CategoryObject : public QObject
{
Q_OBJECT

public:
	CategoryObject();
	virtual ~CategoryObject();

	inline QString name() const { return mName; }
	inline QUrl logoUrl() const { return mLogoUrl; }
	inline int numChannels() const { return mNumChannels; }
	inline int numViewers() const { return mNumViewers; }
	inline bool followed() const { return mFollowed; }

	virtual QString logoCacheString() const;

signals:
	void statsChanged();

protected:
	void setStats(int pChannels, int pViewers);

	QString mName;
	QUrl    mLogoUrl;
	int     mNumChannels;
	int     mNumViewers;
	bool    mFollowed;
};

#endif // CORE_CATEGORY_OBJECT_H_