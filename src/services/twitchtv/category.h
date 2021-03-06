#ifndef SERVICES_TWITCHTV_CATEGORY_H_
#define SERVICES_TWITCHTV_CATEGORY_H_

#include <QVariant>

#include "../../core/i_category.h"

namespace twitchtv
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

	QUrl categoryUrl(UrlType pType) const override;
	void setFollowed(bool pFollow) override;

	void updateFromVariant(QVariant pData);
	void updateFlag(Flag pFlag, bool pEnabled);

private:
	int mGiantBombId;
	QUrl mLogoUrl;
};

} // namespace twitchtv

#endif // SERVICES_TWITCHTV_CATEGORY_H_
