#include "category_object_widget.h"
#include "config.h"
#include "ui_category_object_widget.h"

#include <QByteArray>
#include <QImage>
#include <QPixmap>

namespace forms
{

CategoryObjectWidget::CategoryObjectWidget(CategoryObject * pObject, QWidget * parent) : QWidget(parent), mCategory(pObject)
{
	ui = new Ui::CategoryObjectWidget();
	ui->setupUi(this);

	QString lName = mCategory->name();
	if (lName.length() > 20)
		lName = QString("%1...").arg(lName.left(20));

	ui->btnCategory->setText(lName);
	ui->btnCategory->setToolTip(mCategory->name());

	connect(mCategory, &CategoryObject::statsChanged, this, &CategoryObjectWidget::updateFromObject);
	updateFromObject();
}

CategoryObjectWidget::~CategoryObjectWidget()
{
	delete ui;
	delete mCategory;
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

void CategoryObjectWidget::updateFromObject()
{
	ui->lblChannels->setText(tr("%n channel(s)", 0, mCategory->numChannels()));
	ui->lblViewers->setText(tr("%n viewer(s)", 0, mCategory->numViewers()));
}

} // namespace forms
