#ifndef SERVICES_TWITCHTV3_CHANNEL_H_
#define SERVICES_TWITCHTV3_CHANNEL_H_

#include "core/channel_object.h"

namespace twitchtv3
{

class Profile;
class Channel : public ChannelObject
{
public:
	Channel(Profile & pProfile, QString pName);
	Channel(Profile & pProfile, QVariant const & pValue, bool pFollowing = false);
	~Channel();

	QString logoCacheString() const override;
	ChannelChat * chat() override;
	void requestUpdate() override;
	void updateStreamSettings(QString pTitle, QString pCategory, bool pMature, int pDelay) override;
	QUrl getStreamUrl(UrlType pType) override;

	void updateFromVariant(QVariant const & pValue);

private:
	Profile & mProfile;
};

} // namespace twitchtv3

#endif // SERVICES_TWITCHTV3_CHANNEL_H_
