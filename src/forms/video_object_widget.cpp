#include "video_object_widget.h"
#include "config.h"
#include "ui_video_object_widget.h"
#include <QByteArray>
#include <QImage>
#include <QPixmap>

#include "../core/i_channel.h"
#include "../core/i_user.h"
#include "../core/i_profile.h"
#include "../core/i_video.h"

namespace forms
{

VideoObjectWidget::VideoObjectWidget(IVideo & pObject, QWidget * parent) : QWidget(parent), mVideo(pObject)
{
	ui = new Ui::VideoObjectWidget();
	ui->setupUi(this);

	ui->btnVideo->setText(pObject.id().toString());

	connect(&mVideo, &IVideo::infoUpdated, this, &VideoObjectWidget::updateFromObject);
	updateFromObject();

	QUrl lPreviewUrl = mVideo.videoUrl(IVideo::URL_PREVIEW_IMAGE);
	if (lPreviewUrl.isValid())
	{
		mVideo.channel().owner().profile().downloadLogo(lPreviewUrl, [this] (QByteArray const & pData)
		{
			setLogo(pData);
		});
	}
}

VideoObjectWidget::~VideoObjectWidget()
{
	delete ui;
}

void VideoObjectWidget::setLogo(QByteArray const & pData)
{
	QImage lImage = QImage::fromData(pData);
	if (!lImage.isNull())
	{
		QPixmap lPixmap = QPixmap::fromImage(std::move(lImage));

		if (lPixmap.width() > ui->btnVideo->maximumWidth())
			lPixmap = lPixmap.scaledToWidth(ui->btnVideo->maximumWidth());
		if (lPixmap.height() > ui->btnVideo->maximumHeight())
			lPixmap = lPixmap.scaledToHeight(ui->btnVideo->maximumHeight());

		ui->btnVideo->setText(QString());
		ui->btnVideo->setIcon(lPixmap);
		ui->btnVideo->setIconSize(lPixmap.size());
	}
}

bool VideoObjectWidget::operator< (VideoObjectWidget const & pOther) const
{
	return mVideo < pOther.mVideo;
}

void VideoObjectWidget::updateFromObject()
{
	ui->lblTitle->setText(mVideo.title());
	ui->lblDescription->setText(mVideo.description());
	ui->lblCategory->setText(mVideo.category());
	ui->lblViews->setText(QString::number(mVideo.views()));
	ui->lblDate->setText(mVideo.recorded().toLocalTime().toString(Qt::SystemLocaleShortDate));
}

void VideoObjectWidget::on_btnVideo_clicked()
{
	emit clicked(&mVideo);
}

} // namespace forms
