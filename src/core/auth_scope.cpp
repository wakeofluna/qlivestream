#include "config.h"
#include "auth_scope.h"
#include "first_time.h"
#include <QString>
#include <QTextStream>

QString AuthScope::description() const
{
	switch (mScope)
	{
		case user_read:
			return QT_TRANSLATE_NOOP("AuthScope", "Read access to non-public user information, such as email address.");

		case user_blocks_edit:
			return QT_TRANSLATE_NOOP("AuthScope", "Ability to ignore or unignore on behalf of a user.");

		case user_blocks_read:
			return QT_TRANSLATE_NOOP("AuthScope", "Read access to a user's list of ignored users.");

		case user_follows_edit:
			return QT_TRANSLATE_NOOP("AuthScope", "Access to manage a user's followed channels.");

		case channel_read:
			return QT_TRANSLATE_NOOP("AuthScope", "Read access to non-public channel information, including email address and stream key.");

		case channel_editor:
			return QT_TRANSLATE_NOOP("AuthScope", "Write access to channel metadata (game, status, etc).");

		case channel_commercial:
			return QT_TRANSLATE_NOOP("AuthScope", "Access to trigger commercials on channel.");

		case channel_stream:
			return QT_TRANSLATE_NOOP("AuthScope", "Ability to reset a channel's stream key.");

		case channel_subscriptions:
			return QT_TRANSLATE_NOOP("AuthScope", "Read access to all subscribers to your channel.");

		case user_subscriptions:
			return QT_TRANSLATE_NOOP("AuthScope", "Read access to subscriptions of a user.");

		case channel_check_subscription:
			return QT_TRANSLATE_NOOP("AuthScope", "Read access to check if a user is subscribed to your channel.");

		case chat_login:
			return QT_TRANSLATE_NOOP("AuthScope", "Ability to log into chat and send messages.");

		case max_auth_scope:
			return QT_TRANSLATE_NOOP("AuthScope", "Invalid scope.");
	}
}

QString AuthScope::toString() const
{
	switch (mScope)
	{
		case user_read:
			return "user_read";

		case user_blocks_edit:
			return "user_blocks_edit";

		case user_blocks_read:
			return "user_blocks_read";

		case user_follows_edit:
			return "user_follows_edit";

		case channel_read:
			return "channel_read";

		case channel_editor:
			return "channel_editor";

		case channel_commercial:
			return "channel_commercial";

		case channel_stream:
			return "channel_stream";

		case channel_subscriptions:
			return "channel_subscriptions";

		case user_subscriptions:
			return "user_subscriptions";

		case channel_check_subscription:
			return "channel_check_subscription";

		case chat_login:
			return "chat_login";

		case max_auth_scope:
			Q_ASSERT_X(false, "AuthScope", "invalid scope");
			return QString();
	}
}

AuthScope AuthScope::fromString(QString pString)
{
	for (size_t i = 0; i < max_auth_scope; ++i)
	{
		AuthScope lScope((Scope)i);
		if (lScope.toString() == pString)
			return lScope;
	}

	return AuthScope(max_auth_scope);
}

bool AuthScope::isDefault() const
{
	switch (mScope)
	{
		case user_blocks_edit:
		case user_blocks_read:
		case channel_editor:
		case channel_commercial:
		case channel_subscriptions:
		case channel_check_subscription:
		case chat_login:
			return true;

		case user_read:
		case user_follows_edit:
		case user_subscriptions:
		case channel_read:
		case channel_stream:
		case max_auth_scope:
			return false;
	}
}

AuthScopes::AuthScopes()
{
}

AuthScopes::~AuthScopes()
{
}

void AuthScopes::setAll()
{
	c.set();
}

QString AuthScopes::toString() const
{
	QString lString;
	QTextStream lStream(&lString);
	FirstTime lFirst;

	for (size_t i = 0; i < AuthScope::max_auth_scope; ++i)
	{
		AuthScope lScope((AuthScope::Scope)i);
		if (!test(lScope))
			continue;

		if (lFirst)
			lStream << '+';

		lStream << lScope.toString();
	}

	return lString;
}
