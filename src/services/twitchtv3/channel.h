#ifndef SERVICES_TWITCHTV3_CHANNEL_H_
#define SERVICES_TWITCHTV3_CHANNEL_H_

#include "core/channel_object.h"

namespace twitchtv3
{

class ChatChannel;
class Profile;
class Channel : public ChannelObject
{
public:
	explicit Channel(Profile & pProfile, QString pName);
	explicit Channel(Profile & pProfile, QVariant const & pValue, bool pFollowing);
	~Channel();

	Profile * profile() const;

	QString logoCacheString() const override;
	ChannelChat * chat() override;
	void requestUpdate() override;
	void updateStreamSettings(QString pTitle, QString pCategory, bool pMature, int pDelay) override;
	QUrl getStreamUrl(UrlType pType) override;

	void updateFromVariant(QVariant const & pValue);

private:
	ChatChannel * mChat;
};

} // namespace twitchtv3

#endif // SERVICES_TWITCHTV3_CHANNEL_H_
