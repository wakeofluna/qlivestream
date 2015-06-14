#ifndef SERVICES_TWITCHTV3_CHANNEL_H_
#define SERVICES_TWITCHTV3_CHANNEL_H_

#include "core/channel_object.h"

namespace twitchtv3
{

class Channel : public ChannelObject
{
public:
	Channel(QString pName);
	Channel(QVariant const & pValue, bool pFollowing = false);
	~Channel();

	QString logoCacheString() const override;
	ChannelChat * chat() override;

	void updateFromVariant(QVariant const & pValue);
};

} // namespace twitchtv3

#endif // SERVICES_TWITCHTV3_CHANNEL_H_
