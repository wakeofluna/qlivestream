#include "config.h"
#include "main_window_videos.h"
#include "ui_main_window_videos.h"
#include "forms/download_window.h"
#include "forms/flowing_layout.h"
#include "forms/video_object_widget.h"
#include "forms/form_helpers.h"
#include "misc.h"

#include <algorithm>
#include <QScrollBar>
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QProcess>

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
	// TODO merge with logic from channel

	QWidget * lPopup = new QWidget(this, Qt::Popup);
	QPushButton * lButton;

	lPopup->setAttribute(Qt::WA_DeleteOnClose);
	lPopup->setLayout(new QBoxLayout(QBoxLayout::TopToBottom, lPopup));

	lButton = new QPushButton(lPopup);
	lButton->setText("Watch!");
	QObject::connect(lButton, &QPushButton::clicked, [pVideo,lPopup]
	{
		lPopup->setEnabled(false);
		QUrl lUrl = pVideo->videoUrl(IVideo::URL_VIDEO_DIRECT);
		lPopup->close();

		if (!lUrl.isValid())
			return;

		QString lProgram;
		QStringList lArguments;
		lProgram = "/usr/bin/mpv";
		lArguments << "--untimed";
		lArguments << "--quiet";
		lArguments << lUrl.toString(QUrl::FullyEncoded);

		QProcess * lProcess = new QProcess();
		QObject::connect(lProcess, (void (QProcess::*) (int))&QProcess::finished, [lProcess] (int pExitCode) { lProcess->deleteLater(); });
		lProcess->setProcessChannelMode(QProcess::ForwardedChannels);
		lProcess->setProgram(lProgram);
		lProcess->setArguments(lArguments);
		lProcess->start();
	});
	lPopup->layout()->addWidget(lButton);

	lButton = new QPushButton(lPopup);
	lButton->setText("Open in browser");
	QObject::connect(lButton, &QPushButton::clicked, [pVideo,lPopup]
	{
		lPopup->setEnabled(false);
		QUrl lUrl = pVideo->videoUrl(IVideo::URL_VIDEO_WEBSITE);
		lPopup->close();

		if (!lUrl.isValid())
			return;

		QDesktopServices::openUrl(lUrl);
	});
	lPopup->layout()->addWidget(lButton);

	lButton = new QPushButton(lPopup);
	lButton->setText("Download ...");
	QObject::connect(lButton, &QPushButton::clicked, [pVideo,lPopup]
	{
		lPopup->setEnabled(false);
		IDownloader * lDownloader = pVideo->downloader();
		lPopup->close();

		if (lDownloader == nullptr)
			return;

		DownloadWindow * lWindow = new DownloadWindow(lDownloader);
		lWindow->setAttribute(Qt::WA_DeleteOnClose);
		lWindow->show();
	});
	lPopup->layout()->addWidget(lButton);

	lPopup->move(QCursor::pos() - QPoint(5, 10));
	lPopup->show();
}

} // namespace forms
