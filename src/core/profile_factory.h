#ifndef CORE_PROFILE_FACTORY_H_
#define CORE_PROFILE_FACTORY_H_

#include <memory>
#include <QString>

#include "../config.h"

template<typename T> class Initializer;

class IProfile;

class COREDLL ProfileFactory
{
public:
	typedef const char * (*NameFunc)();
	typedef IProfile * (*CreateFunc)();

protected:
	inline ProfileFactory() {}
	inline ~ProfileFactory() {}

	void registerService(QString pService, CreateFunc pFunction);
	std::unique_ptr<IProfile> createProfile(QString pService) const;
	QStringList listServices() const;

private:
	friend class Initializer<ProfileFactory>;
	static void initialize();
	static void finalize();
	static bool loadModule(QString pModuleName);
};

#endif // CORE_PROFILE_FACTORY_H_
