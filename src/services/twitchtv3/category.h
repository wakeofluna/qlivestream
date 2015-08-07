#ifndef SERVICES_TWITCHTV3_CATEGORY_H_
#define SERVICES_TWITCHTV3_CATEGORY_H_

#include <QVariant>

#include "../../core/i_category.h"

namespace twitchtv3
{

class Profile;
class Category : public ICategory
{
public:
	Category(Profile & pProfile, QString pName);
	~Category();

	Profile & profile() const;

	QString logoCacheString() const override;
	void rollupChannels() override;

	void setFollowed(bool pFollow) override;

	void updateFromVariant(QVariant pData);
	void updateFlag(Flag pFlag, bool pEnabled);

private:
	int mGiantBombId;
};

} // namespace twitchtv3

#endif // SERVICES_TWITCHTV3_CATEGORY_H_
