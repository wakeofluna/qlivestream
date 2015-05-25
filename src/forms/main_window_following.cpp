#include "config.h"
#include "main_window_following.h"
#include "ui_main_window_following.h"
#include "core/channel_object.h"
#include "core/profile.h"
#include "forms/flowing_layout.h"

#include <QScrollBar>

namespace forms
{

MainWindowFollowing::MainWindowFollowing(Profile & pProfile, QWidget * parent) : QWidget(parent), mProfile(pProfile)
{
	ui = new Ui::MainWindowFollowing();
	ui->setupUi(this);

	ui->grpOnline->setLayout(new FlowingLayout(ui->grpOnline));
	ui->grpOffline->setLayout(new FlowingLayout(ui->grpOffline));

	connect(ui->btnRefresh, &QPushButton::clicked, this, &MainWindowFollowing::refresh);
	connect(ui->scrArea->verticalScrollBar(), &QScrollBar::valueChanged, this, &MainWindowFollowing::checkRollup);

	mCanRollup = false;
}

MainWindowFollowing::~MainWindowFollowing()
{
	delete ui;
}

void MainWindowFollowing::refresh()
{
	clear();
	rollup();
}

void MainWindowFollowing::rollup()
{
	mCanRollup = false;
}

void MainWindowFollowing::checkRollup(int pSliderValue)
{
	bool lRollup = true;
	lRollup &= mCanRollup;
	lRollup &= (pSliderValue + 200 >= ui->scrArea->verticalScrollBar()->maximum());

	if (lRollup)
		rollup();
}

void MainWindowFollowing::clear()
{
}

void MainWindowFollowing::addData(QList<ChannelObject*> && pChannels)
{
	qDeleteAll(pChannels);
}

} // namespace forms
