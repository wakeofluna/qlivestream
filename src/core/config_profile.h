#ifndef CORE_CONFIG_PROFILE_H_
#define CORE_CONFIG_PROFILE_H_

#include <QString>
#include <QVector>
#include "profile_factory.h"

#include "../config.h"

class QDateTime;

class COREDLL ConfigProfile : public ProfileFactory
{
public:
	typedef QVector<ConfigProfile> List;

public:
	ConfigProfile();
	~ConfigProfile();

	static List listProfiles();
	QString toString() const;

	QDateTime lastAccess() const;
	std::unique_ptr<IProfile> load() const;

	inline int id() const { return mId; }
	inline QString account() const { return mAccount; }
	inline QString service() const { return mService; }
	inline int level() const { return mLevel; }

	void setAccount(QString pAccount);
	void setService(QString pService);
	void setLevel(int pLevel);
	void erase() const;
	void save();

	inline bool operator< (ConfigProfile const & rhs) const { return mLastAccess < rhs.mLastAccess; }

private:
	int     mId;
	QString mAccount;
	QString mToken;
	QString mService;
	int     mLevel;
	int     mLastAccess;
};

#endif // CORE_CONFIG_PROFILE_H_
