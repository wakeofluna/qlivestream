#include "config.h"
#include "profile.h"

const char * ServiceName();

namespace twitchtv3
{

Profile::Profile()
{
}

Profile::~Profile()
{
}

QString Profile::service() const
{
	return ServiceName();
}


} // namespace twitchtv3
