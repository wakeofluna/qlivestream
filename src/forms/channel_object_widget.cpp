#include "config.h"
#include "channel_object_widget.h"
#include "ui_channel_object_widget.h"

#include <QByteArray>
#include <QImage>
#include <QPixmap>

#include "../core/i_category.h"
#include "../core/i_channel.h"
#include "../core/i_profile.h"
#include "../core/i_user.h"

namespace forms
{

ChannelObjectWidget::ChannelObjectWidget(IChannel & pObject, QWidget * parent) : QWidget(parent), mChannel(pObject)
{
	ui = new Ui::ChannelObjectWidget();
	ui->setupUi(this);

	ui->btnChannel->setText(mChannel.owner().name());
	ui->lblName->setText(mChannel.displayName());

	connect(&mChannel, &IChannel::infoUpdated, this, &ChannelObjectWidget::updateFromObject);
	updateFromObject();

	if (mChannel.logoUrl().isValid())
		accessCache
		(
				mChannel.logoCacheString(),
				[this] (DataCallback && pCallback)
				{
					mChannel.owner().profile().downloadLogo(mChannel.logoUrl(), std::move(pCallback));
				},
				[this] (QByteArray const & pData)
				{
					setLogo(pData);
				}
		);
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

bool ChannelObjectWidget::operator< (ChannelObjectWidget const & pOther) const
{
	return mChannel < pOther.mChannel;
}

void ChannelObjectWidget::updateFromObject()
{
	QString lStatus = mChannel.title();
	if (lStatus.size() > 40)
	{
		ui->lblStatus->setToolTip(lStatus);
		lStatus = QString("%1...").arg(lStatus.left(37));
	}
	else
		ui->lblStatus->setToolTip(QString());

	ui->lblStatus->setText(lStatus);
	ui->lblCategory->setText(mChannel.category() ? mChannel.category()->name() : QString());
	ui->lblFollowers->setText(QString::number(mChannel.numFollowers()));

	int lNumViewers = mChannel.numViewers();
	ui->lblViewersLabel->setVisible(lNumViewers >= 0);
	ui->lblViewers->setVisible(lNumViewers >= 0);
	ui->lblViewers->setText(QString::number(lNumViewers));
}

void ChannelObjectWidget::on_btnChannel_clicked()
{
	emit clicked(&mChannel);
}

} // namespace forms
