#ifndef CORE_AUTH_SCOPE_H_
#define CORE_AUTH_SCOPE_H_

#include "class_bitset.h"

class QString;

// As defined on https://github.com/justintv/Twitch-API/blob/master/authentication.md#scopes
class AuthScope
{
public:
	enum Scope
	{
		// Read access to non-public user information, such as email address.
		user_read,

		// Ability to ignore or unignore on behalf of a user.
		user_blocks_edit,

		// Read access to a user's list of ignored users.
		user_blocks_read,

		// Access to manage a user's followed channels.
		user_follows_edit,

		// Read access to non-public channel information, including email address and stream key.
		channel_read,

		// Write access to channel metadata (game, status, etc).
		channel_editor,

		// Access to trigger commercials on channel.
		channel_commercial,

		// Ability to reset a channel's stream key.
		channel_stream,

		// Read access to all subscribers to your channel.
		channel_subscriptions,

		// Read access to subscriptions of a user.
		user_subscriptions,

		// Read access to check if a user is subscribed to your channel.
		channel_check_subscription,

		// Ability to log into chat and send messages.
		chat_login,

		// Terminator value
		max_auth_scope
	};

	typedef Scope enum_type;
	static const enum_type max = max_auth_scope;

public:
	inline AuthScope(enum_type pScope) : mScope(pScope) {}
	inline operator enum_type() const { return mScope; }

	QString description() const;
	QString toString() const;
	static AuthScope fromString(QString pString);

	inline bool valid() const { return mScope >= 0 && mScope < max; }

private:
	Scope mScope;
};

class AuthScopes : public ClassBitset<AuthScope>
{
public:
	AuthScopes();
	~AuthScopes();

	QString toString() const;
	static AuthScopes fromString(QString pString);
};

#endif // CORE_AUTH_SCOPE_H_
