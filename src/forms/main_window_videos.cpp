#include "config.h"
#include "main_window_videos.h"
#include "ui_main_window_videos.h"
#include "forms/flowing_layout.h"
#include "forms/video_object_widget.h"
#include "forms/form_helpers.h"
#include "misc.h"

#include <algorithm>
#include <QScrollBar>
#include <QDesktopServices>

#include "../core/i_profile.h"
#include "../core/i_video.h"

namespace forms
{

MainWindowVideos::MainWindowVideos(IChannel & pChannel, QWidget * parent) : QWidget(parent), mChannel(pChannel)
{
	ui = new Ui::MainWindowVideos();
	ui->setupUi(this);

	ui->scrAreaContents->setLayout(new FlowingLayout(ui->scrAreaContents, qobject_less<QWidget, VideoObjectWidget>()));

	connect(ui->btnRefresh, &QPushButton::clicked, this, &MainWindowVideos::refresh);
	connect(ui->scrArea->verticalScrollBar(), &QScrollBar::valueChanged, this, &MainWindowVideos::checkRollup);
	connect(&mChannel, &IChannel::videosUpdated, this, &MainWindowVideos::onVideosUpdated);

	refresh();
}

MainWindowVideos::~MainWindowVideos()
{
	qDeleteAll(mVideos);
	delete ui;
}

void MainWindowVideos::refresh()
{
	mChannel.resetVideos();
}

void MainWindowVideos::rollup()
{
	mChannel.rollupVideos();
}

void MainWindowVideos::checkRollup(int pSliderValue)
{
	if (pSliderValue + 200 >= ui->scrArea->verticalScrollBar()->maximum())
		rollup();
}

void MainWindowVideos::onVideosUpdated()
{
	addToFlowLayout(mChannel.videos(), mVideos, ui->scrAreaContents->layout(), this, &MainWindowVideos::selected);
}

void MainWindowVideos::selected(IVideo * pVideo)
{
	QUrl lUrl;
	lUrl = pVideo->videoUrl(IVideo::URL_VIDEO_WEBSITE);
	if (lUrl.isValid())
		QDesktopServices::openUrl(lUrl);

}

} // namespace forms
