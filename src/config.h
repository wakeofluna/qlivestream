#ifndef _CONFIG_H_
#define _CONFIG_H_

#define APP_NAME "Qtwitch"

#define SETTINGS_DEF QSettings::IniFormat, QSettings::UserScope, QApplication::organizationName(), QApplication::applicationName()
#define SETTINGS_PTR(x) x = new QSettings(SETTINGS_DEF)
#define SETTINGS(x) QSettings x(SETTINGS_DEF)

// Helper for lambda captures
#if defined(CDT_PARSER) || __cplusplus < 201402L
#define CAPTURE(x) x
#else
#define CAPTURE(x) x = std::move(x)
#endif

#endif // _CONFIG_H_
