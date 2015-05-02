#include "config.h"
#include "profile.h"

#include <QApplication>
#include <QSettings>
#include <QStringList>
#include <QVariant>

Profile::Profile()
{
}

Profile::~Profile()
{
}

bool Profile::isValid() const
{
	return !mAccount.isEmpty();
}

QStringList Profile::listProfiles()
{
	SETTINGS(settings);

	settings.beginGroup("Profiles");
	return settings.childGroups();
}

Profile Profile::load(QString pName)
{
	SETTINGS(settings);

	Profile lProfile;
	settings.beginGroup("Profiles");
	settings.beginGroup(pName);
	lProfile.mName = pName;
	lProfile.mAccount = settings.value("account").toString();

	for (int i = 0; i < AuthScope::max; ++i)
	{
		AuthScope lScope((AuthScope::Scope)i);
		bool lSet = settings.value(lScope.toString(), false).toBool();
		if (lSet)
			lProfile.mRequested.set(lScope);
	}

	return lProfile;
}

void Profile::save() const
{
	if (!isValid())
		return;

	SETTINGS(settings);

	settings.beginGroup("Profiles");
	settings.beginGroup(mName);
	settings.setValue("account", mAccount);

	for (int i = 0; i < AuthScope::max; ++i)
	{
		AuthScope lScope((AuthScope::Scope)i);
		settings.setValue(lScope.toString(), mRequested.test(lScope));
	}
}
