#include "category_object_widget.h"
#include "config.h"
#include "ui_category_object_widget.h"
#include <QByteArray>
#include <QImage>
#include <QPixmap>

#include "../core/i_category.h"
#include "../core/i_profile.h"

namespace forms
{

CategoryObjectWidget::CategoryObjectWidget(ICategory & pObject, QWidget * parent) : QWidget(parent), mCategory(pObject)
{
	ui = new Ui::CategoryObjectWidget();
	ui->setupUi(this);

	QString lName = mCategory.name();
	if (lName.length() > 20)
		lName = QString("%1...").arg(lName.left(20));

	ui->btnCategory->setText(lName);
	ui->btnCategory->setToolTip(mCategory.name());

	connect(&mCategory, &ICategory::infoUpdated, this, &CategoryObjectWidget::updateFromObject);
	updateFromObject();

	if (mCategory.categoryUrl(ICategory::URL_LOGO).isValid())
		accessCache
		(
				mCategory.logoCacheString(),
				[this] (DataCallback && pCallback)
				{
					mCategory.profile().downloadLogo(mCategory.categoryUrl(ICategory::URL_LOGO), std::move(pCallback));
				},
				[this] (QByteArray const & pData)
				{
					setLogo(pData);
				}
		);
}

CategoryObjectWidget::~CategoryObjectWidget()
{
	delete ui;
}

void CategoryObjectWidget::setLogo(QByteArray const & pData)
{
	QImage lImage = QImage::fromData(pData);
	if (!lImage.isNull())
	{
		QPixmap lPixmap = QPixmap::fromImage(std::move(lImage));
		ui->btnCategory->setText(QString());
		ui->btnCategory->setIcon(lPixmap);
		ui->btnCategory->setIconSize(lPixmap.size());
	}
}

bool CategoryObjectWidget::operator< (CategoryObjectWidget const & pOther) const
{
	return mCategory < pOther.mCategory;
}

void CategoryObjectWidget::updateFromObject()
{
	int lChannels = mCategory.numChannels();
	ui->lblChannels->setVisible(lChannels >= 0);
	ui->lblChannels->setText(tr("%n channel(s)", 0, lChannels));

	int lViewers = mCategory.numViewers();
	ui->lblViewers->setVisible(lViewers >= 0);
	ui->lblViewers->setText(tr("%n viewer(s)", 0, lViewers));
}

void CategoryObjectWidget::on_btnCategory_clicked()
{
	emit clicked(&mCategory);
}

} // namespace forms
