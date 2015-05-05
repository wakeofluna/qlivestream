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
	lProfile.mAuthToken = settings.value("token").toString();

	for (int i = 0; i < AuthScope::max; ++i)
	{
		AuthScope lScope((AuthScope::Scope)i);
		bool lSet = settings.value(lScope.toString(), false).toBool();
		if (lSet)
			lProfile.mRequested.set(lScope);
	}

	return lProfile;
}

void Profile::erase(QString pName)
{
	SETTINGS(settings);

	eraseImpl(settings, pName);
}

void Profile::save() const
{
	saveAndReplace(mName);
}

void Profile::saveAndReplace(QString pOldName) const
{
	SETTINGS(settings);

	if (!pOldName.isEmpty())
		eraseImpl(settings, pOldName);

	if (!mName.isEmpty())
		saveImpl(settings);
}

void Profile::eraseImpl(QSettings & pSettings, QString pName)
{
	pSettings.beginGroup("Profiles");
	pSettings.remove(pName);
	pSettings.endGroup();
}

void Profile::saveImpl(QSettings & pSettings) const
{
	pSettings.beginGroup("Profiles");
	pSettings.beginGroup(mName);
	pSettings.setValue("account", mAccount);
	pSettings.setValue("token", mAuthToken);

	for (int i = 0; i < AuthScope::max; ++i)
	{
		AuthScope lScope((AuthScope::Scope)i);
		pSettings.setValue(lScope.toString(), mRequested.test(lScope));
	}

	pSettings.endGroup();
	pSettings.endGroup();
}
