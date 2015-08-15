#ifndef SERVICES_TWITCHTV3_CHANNEL_H_
#define SERVICES_TWITCHTV3_CHANNEL_H_

#include <QString>
#include <QVariant>

#include "../../core/i_channel.h"

namespace twitchtv3
{

class ChannelChat;
class Profile;
class User;
class Channel : public IChannel
{
public:
	explicit Channel(User & pUser);
	~Channel();

	Profile & profile() const;
	User & owner() const;
	QString name() const;

	QString logoCacheString() const override;
	IChannelChat * chat() override;
	QUrl streamUrl(UrlType pType) override;

	void refresh() override;
	void modifyStreamSettings(QString pTitle, ICategory * pCategory, bool pMature, int pDelay) override;
	void setFollowed(bool pFollow) override;
	void rollupVideos() override;

	void updateFromVariant(QVariant const & pValue);

private:
	IVideo * processVideoObject(QVariant pVideo);

	ChannelChat * mChat;
};

} // namespace twitchtv3

#endif // SERVICES_TWITCHTV3_CHANNEL_H_
