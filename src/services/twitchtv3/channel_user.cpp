#include "channel_user.h"

#include <QChar>
#include <QObject>
#include "channel.h"
#include "profile.h"
#include "user.h"

#include "../../core/i_user.h"
#include "../../misc.h"

namespace
{

struct ColorDef
{
	const char * name;
	IChannelUser::Color color;
};

static constexpr ColorDef _predefined[] = {
		{ "Red",         { 0xFF, 0x00, 0x00 } },
		{ "Blue",        { 0x00, 0x00, 0xFF } },
		{ "Green",       { 0x00, 0x80, 0x00 } },
		{ "FireBrick",   { 0xB2, 0x22, 0x22 } },
		{ "Coral",       { 0xFF, 0x7F, 0x50 } },
		{ "YellowGreen", { 0x9A, 0xCD, 0x32 } },
		{ "OrangeRed",   { 0xFF, 0x45, 0x00 } },
		{ "SeaGreen",    { 0x2E, 0x8B, 0x57 } },
		{ "GoldenRod",   { 0xDA, 0xA5, 0x20 } },
		{ "Chocolate",   { 0xD2, 0x69, 0x1E } },
		{ "CadetBlue",   { 0x5F, 0x9E, 0xA0 } },
		{ "DodgerBlue",  { 0x1E, 0x90, 0xFF } },
		{ "HotPink",     { 0xFF, 0x69, 0xB4 } },
		{ "BlueViolet",  { 0x8A, 0x2B, 0xE2 } },
		{ "SpringGreen", { 0x00, 0xFF, 0x7F } }
};

static constexpr int _numPredefined = sizeof(_predefined) / sizeof(*_predefined);

}

namespace twitchtv3
{

ChannelUser::ChannelUser(Channel & pChannel, User & pUser) : IChannelUser(pChannel, pUser)
{
	QObject::connect(&pUser, &IUser::infoUpdated, this, &IChannelUser::infoUpdated);

	QString lName = pUser.name();
	if (!lName.isEmpty())
	{
		int lSum = lName.at(0).toLatin1() + lName.at(lName.length()-1).toLatin1();
		mColor = _predefined[lSum % _numPredefined].color;
	}
}

ChannelUser::~ChannelUser()
{
}

Profile & ChannelUser::profile() const
{
	return user().profile();
}

Channel & ChannelUser::channel() const
{
	return static_cast<Channel&>(mChannel);
}

User & ChannelUser::user() const
{
	return static_cast<User&>(mUser);
}

void ChannelUser::updateFromTags(ChatServer::Tags const & pTags)
{
	QString lDisplayName = pTags.value("display-name");
	bool lSubscriber = pTags.value("subscriber") == "1";
	bool lTurbo = pTags.value("turbo") == "1";
	QString lUserType = pTags.value("user-type");
	Color lColor = parseColor(pTags.value("color"));

	user().updateFlag(IUser::Flag::VETERAN, lTurbo);

	bool lChanged = false;
	lChanged |= updateIfChanged(mDisplayName, lDisplayName, !lDisplayName.isEmpty());
	lChanged |= updateIfChanged(mUserType, lUserType, !lUserType.isEmpty());
	lChanged |= updateIfChanged(mColor, lColor, lColor.isValid());
	lChanged |= updateIfChanged<Flag>(mFlags, Flag::SUBSCRIBER, lSubscriber);
	lChanged |= updateIfChanged<Flag>(mFlags, Flag::MODERATOR, lUserType == "mod");

	if (lChanged)
		emit infoUpdated();
}

void ChannelUser::updateFlag(Flag pFlag, bool pEnabled)
{
	if (updateIfChanged<Flag>(mFlags, pFlag, pEnabled))
		emit infoUpdated();
}

ChannelUser::Color ChannelUser::parseColor(QString pInput)
{
	Color lColor;

	if (pInput.length() != 7)
		return lColor;

	if (pInput[0] != '#')
		return lColor;

	bool ok[3];
	lColor.r = pInput.mid(1, 2).toInt(&ok[0], 16);
	lColor.g = pInput.mid(3, 2).toInt(&ok[1], 16);
	lColor.b = pInput.mid(5, 2).toInt(&ok[2], 16);

	if (!ok[0] || !ok[1] || !ok[2])
		lColor.a = 0;
	else
		lColor.a = 255;

	return lColor;
}

} // namespace twitchtv3
