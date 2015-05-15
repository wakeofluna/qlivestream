#ifndef SERVICES_TWITCHTV3_PROFILE_H_
#define SERVICES_TWITCHTV3_PROFILE_H_

#include "core/profile.h"

namespace twitchtv3
{

class Profile : public ::Profile
{
public:
	Profile();
	~Profile();

	QString service() const override;
};

} // namespace twitchtv3

#endif // SERVICES_TWITCHTV3_PROFILE_H_
