#include "config.h"
#include "profile_factory.h"
#include "core/config_profile.h"

namespace
{
	QList<ProfileFactory::ServicePair> mList;
	bool mSorted;
}

void ProfileFactory::registerService(QString pService, CreateFunc pFunction)
{
	mList.append(ServicePair(pService, pFunction));
	if (mList.size() > 1)
		mSorted = false;
}

Profile::Ptr ProfileFactory::createProfile(ConfigProfile const & pTemplate) const
{
	QString lService = pTemplate.service();

	auto lItem = std::find_if(mList.begin(), mList.end(),
			[lService] (ServicePair const & i) -> bool
			{
				return i.first == lService;
			});

	if (lItem == mList.end())
		return nullptr;

	return Profile::Ptr(lItem->second(pTemplate));
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
}

void ProfileFactory::finalize()
{
	mList.clear();
}
