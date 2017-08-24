#include "profile_factory.h"

#include <QLibrary>
#include <QList>
#include <QStringList>
#include "i_profile.h"


namespace
{
	struct ServiceDef
	{
		inline ServiceDef(QString pName, ProfileFactory::CreateFunc pFunc) : name(pName), func(pFunc) {}
		QString name;
		ProfileFactory::CreateFunc func;
	};

	QList<ServiceDef> mList;
	QStringList mNameList;

	void _registerService(QString pService, ProfileFactory::CreateFunc pFunction)
	{
		mList.append(ServiceDef(pService, pFunction));
		mNameList.clear();
	}
}

void ProfileFactory::registerService(QString pService, CreateFunc pFunction)
{
	_registerService(pService, pFunction);
}

std::unique_ptr<IProfile> ProfileFactory::createProfile(QString pService) const
{
	for (auto & lDef : mList)
		if (lDef.name == pService)
			return std::unique_ptr<IProfile>(lDef.func());

	return nullptr;
}

QStringList ProfileFactory::listServices() const
{
	if (mNameList.isEmpty() && !mList.isEmpty())
	{
		mNameList.reserve(mList.length());

		for (auto & lDef : mList)
			mNameList.append(lDef.name);

		mNameList.sort();
	}

	return mNameList;
}

void ProfileFactory::initialize()
{
	// Hardcoded list of possibly available service plugins
	loadModule("twitchtv");
}

void ProfileFactory::finalize()
{
	mList.clear();
	mNameList.clear();
}

bool ProfileFactory::loadModule(QString pModuleName)
{
	QString lModuleName = QString("lib%1-%2").arg(QString(APP_NAME).toLower()).arg(pModuleName);

	QLibrary lLibrary;
	lLibrary.setLoadHints(QLibrary::PreventUnloadHint);
	lLibrary.setFileName(QString("./%1").arg(lModuleName));
	if (!lLibrary.load())
		return false;

	NameFunc lNameFunc = (NameFunc) lLibrary.resolve("ServiceName");
	CreateFunc lCreateFunc = (CreateFunc) lLibrary.resolve("ServiceFactory");

	if (!lNameFunc || !lCreateFunc)
		return false;

	const char * lName = lNameFunc();
	if (lName == nullptr)
		return false;

	_registerService(lName, lCreateFunc);
	return true;
}
