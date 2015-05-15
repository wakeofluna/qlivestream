#ifndef CORE_PROFILE_FACTORY_H_
#define CORE_PROFILE_FACTORY_H_

#include "core/profile.h"
#include <memory>
#include <QList>
#include <QPair>
#include <QString>

class ConfigProfile;
template <typename T> class Initializer;

class ProfileFactory
{
public:
	typedef Profile * (*CreateFunc)(ConfigProfile const & pTemplate);
	typedef QPair<QString, CreateFunc> ServicePair;
	typedef QList<ServicePair> List;

protected:
	inline ProfileFactory() {}
	inline ~ProfileFactory() {}

	void registerService(QString pService, CreateFunc pFunction);
	Profile::Ptr createProfile(ConfigProfile const & pTemplate) const;
	List const & listServices() const;

private:
	friend class Initializer<ProfileFactory>;
	static void initialize();
	static void finalize();
};

#endif // CORE_PROFILE_FACTORY_H_
