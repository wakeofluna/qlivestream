#ifndef _CONFIG_H_
#define _CONFIG_H_

#define APP_NAME "Qtwitch"

#define SETTINGS_DEF QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName()
#define SETTINGS_PTR(x) x = new QSettings(SETTINGS_DEF)
#define SETTINGS(x) QSettings x(SETTINGS_DEF)

#endif // _CONFIG_H_
