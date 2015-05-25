#ifndef SERVICES_TWITCHTV3_GAMES_OBJECT_H_
#define SERVICES_TWITCHTV3_GAMES_OBJECT_H_

#include "core/category_object.h"
class QVariant;

namespace twitchtv3
{

class GamesObject : public CategoryObject
{
public:
	GamesObject(QVariant const & pValue, bool pFollowing = false);
	~GamesObject();

	QString logoCacheString() const override;

private:
	int mGiantBombId;
};

} // namespace twitchtv3

#endif // SERVICES_TWITCHTV3_GAMES_OBJECT_H_
