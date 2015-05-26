#ifndef CORE_PROFILE_FACTORY_H_
#define CORE_PROFILE_FACTORY_H_

#include "config.h"
#include "core/profile.h"
#include <QPair>

class ConfigProfile;
class QString;
template <typename T> class Initializer;
template <typename T> class QList;

class COREDLL ProfileFactory
{
public:
	typedef const char * (*NameFunc)();
	typedef Profile * (*CreateFunc)();
	typedef QPair<QString, CreateFunc> ServicePair;
	typedef QList<ServicePair> List;

protected:
	inline ProfileFactory() {}
	inline ~ProfileFactory() {}

	void registerService(QString pService, CreateFunc pFunction);
	Profile::UPtr createProfile(QString pService) const;
	List const & listServices() const;

private:
	friend class Initializer<ProfileFactory>;
	static void initialize();
	static void finalize();
	static bool loadModule(QString pModuleName);
};

#endif // CORE_PROFILE_FACTORY_H_
