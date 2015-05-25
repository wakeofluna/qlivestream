#include "config.h"
#include "channel_object_widget.h"
#include "ui_channel_object_widget.h"
#include "core/channel_object.h"

#include <QByteArray>
#include <QImage>
#include <QPixmap>

namespace forms
{

ChannelObjectWidget::ChannelObjectWidget(ChannelObject * pObject, QWidget * parent) : QWidget(parent), mChannel(pObject)
{
	ui = new Ui::ChannelObjectWidget();
	ui->setupUi(this);

	ui->btnChannel->setText(mChannel->name());
	ui->lblName->setText(mChannel->displayName());

	connect(mChannel, &ChannelObject::statsChanged, this, &ChannelObjectWidget::updateFromObject);
	updateFromObject();
}

ChannelObjectWidget::~ChannelObjectWidget()
{
	delete ui;
}

void ChannelObjectWidget::setLogo(QByteArray const & pData)
{
	QImage lImage = QImage::fromData(pData);
	if (!lImage.isNull())
	{
		QPixmap lPixmap = QPixmap::fromImage(std::move(lImage)).scaledToHeight(100);
		ui->btnChannel->setText(QString());
		ui->btnChannel->setIcon(lPixmap);
		ui->btnChannel->setIconSize(lPixmap.size());
	}
}

void ChannelObjectWidget::updateFromObject()
{
	QString lStatus = mChannel->status();
	if (lStatus.size() > 40)
	{
		ui->lblStatus->setToolTip(lStatus);
		lStatus = QString("%1...").arg(lStatus.left(37));
	}
	else
		ui->lblStatus->setToolTip(QString());

	ui->lblStatus->setText(lStatus);
	ui->lblCategory->setText(mChannel->category());
	ui->lblFollowers->setText(QString::number(mChannel->numFollowers()));
}

void ChannelObjectWidget::on_btnChannel_clicked()
{
	emit clicked(mChannel);
}

} // namespace forms
