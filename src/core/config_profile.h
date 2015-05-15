#ifndef CORE_CONFIG_PROFILE_H_
#define CORE_CONFIG_PROFILE_H_

#include "core/profile.h"
#include "core/profile_factory.h"
#include <QString>
#include <QVector>

class QDateTime;

namespace forms
{
	class EditProfile;
}

class ConfigProfile : public Profile, public ProfileFactory
{
public:
	typedef QVector<ConfigProfile> List;

public:
	ConfigProfile();
	~ConfigProfile();

	static List listProfiles();
	QString toString() const;

	QString service() const override;
	QDateTime lastAccess() const;
	Profile::Ptr load() const;

	inline bool operator< (ConfigProfile const & rhs) const { return mLastAccess < rhs.mLastAccess; }

private:
	friend class forms::EditProfile;

	void setAccount(QString pAccount);
	void setService(QString pService);
	void setLevel(Level pLevel);
	void erase() const;
	void save();

	QString mService;
	int mLastAccess;
};

#endif // CORE_CONFIG_PROFILE_H_
