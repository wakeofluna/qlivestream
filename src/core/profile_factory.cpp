#include "config.h"
#include "profile_factory.h"
#include "core/config_profile.h"
#include <QLibrary>

namespace
{
	QList<ProfileFactory::ServicePair> mList;
	bool mSorted;

	void _registerService(QString pService, ProfileFactory::CreateFunc pFunction)
	{
		mList.append(ProfileFactory::ServicePair(pService, pFunction));
		if (mList.size() > 1)
			mSorted = false;
	}
}

void ProfileFactory::registerService(QString pService, CreateFunc pFunction)
{
	_registerService(pService, pFunction);
}

Profile::Ptr ProfileFactory::createProfile(QString pService) const
{
	auto lItem = std::find_if(mList.begin(), mList.end(),
			[pService] (ServicePair const & i) -> bool
			{
				return i.first == pService;
			});

	if (lItem == mList.end())
		return nullptr;

	return Profile::Ptr(lItem->second());
}

ProfileFactory::List const & ProfileFactory::listServices() const
{
	if (!mSorted)
		std::sort(mList.begin(), mList.end(),
				[] (ServicePair const & lhs, ServicePair const & rhs) -> bool
				{
					return lhs.first < rhs.first;
				});

	return mList;
}

void ProfileFactory::initialize()
{
	mList.clear();
	mSorted = true;

	// Hardcoded list of possibly available service plugins
	loadModule("twitchtv3");
}

void ProfileFactory::finalize()
{
	mList.clear();
}

bool ProfileFactory::loadModule(QString pModuleName)
{
	QString lModuleName = QString(APP_NAME).toLower();
	lModuleName += '-' + pModuleName;

	QLibrary lLibrary;
	lLibrary.setLoadHints(QLibrary::PreventUnloadHint);
	lLibrary.setFileName("./" + lModuleName);
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
