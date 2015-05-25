#ifndef SERVICES_TWITCHTV3_CHANNELS_OBJECT_H_
#define SERVICES_TWITCHTV3_CHANNELS_OBJECT_H_

#include "core/channel_object.h"

namespace twitchtv3
{

class ChannelsObject : public ChannelObject
{
public:
	ChannelsObject(QVariant const & pValue, bool pFollowing = false);
	~ChannelsObject();

	QString logoCacheString() const override;
};

} // namespace twitchtv3

#endif // SERVICES_TWITCHTV3_CHANNELS_OBJECT_H_
