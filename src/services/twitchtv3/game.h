#ifndef SERVICES_TWITCHTV3_GAME_H_
#define SERVICES_TWITCHTV3_GAME_H_

#include "core/category_object.h"
class QVariant;

namespace twitchtv3
{

class Profile;
class Game : public CategoryObject
{
public:
	explicit Game(Profile & pProfile, QVariant const & pValue, bool pFollowing);
	~Game();

	Profile * profile() const;

	QString logoCacheString() const override;

private:
	int mGiantBombId;
};

} // namespace twitchtv3

#endif // SERVICES_TWITCHTV3_GAME_H_
