#include "config.h"
#include "profile.h"

#include <QApplication>
#include <QSettings>
#include <QStringList>

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
}
