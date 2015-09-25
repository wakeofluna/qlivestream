#include "config.h"
#include "main_window_following.h"
#include "ui_main_window_following.h"
#include "forms/channel_object_widget.h"
#include "forms/flowing_layout.h"
#include "forms/form_helpers.h"

#include <QScrollBar>
#include "../core/i_channel.h"
#include "../core/i_profile.h"

namespace forms
{

MainWindowFollowing::MainWindowFollowing(IProfile & pProfile, QWidget * parent) : QWidget(parent), mProfile(pProfile)
{
	ui = new Ui::MainWindowFollowing();
	ui->setupUi(this);

	ui->grpOnline->setLayout(new FlowingLayout(ui->grpOnline, qobject_less<QWidget, ChannelObjectWidget>()));
	ui->grpOffline->setLayout(new FlowingLayout(ui->grpOffline, qobject_less<QWidget, ChannelObjectWidget>()));

	connect(ui->btnRefresh, &QPushButton::clicked, this, &MainWindowFollowing::refresh);
	connect(ui->scrArea->verticalScrollBar(), &QScrollBar::valueChanged, this, &MainWindowFollowing::checkRollup);
	connect(&mProfile, &IProfile::followedChannelsUpdated, this, &MainWindowFollowing::onChannelsUpdated);

	mProfile.resetFollowedChannels();
}

MainWindowFollowing::~MainWindowFollowing()
{
	qDeleteAll(mOnlineWidgets);
	qDeleteAll(mOfflineWidgets);
	delete ui;
}

void MainWindowFollowing::refresh()
{
	mProfile.resetFollowedChannels();
}

void MainWindowFollowing::rollup()
{
	mProfile.rollupFollowedChannels();
}

void MainWindowFollowing::checkRollup(int pSliderValue)
{
	if (pSliderValue + 200 >= ui->scrArea->verticalScrollBar()->maximum())
		rollup();
}

void MainWindowFollowing::onChannelsUpdated()
{
	QList<IChannel *> lOnline;
	QList<IChannel *> lOffline;

	for (IChannel * lChannel : mProfile.followedChannels())
	{
		if (lChannel->isOnline())
			lOnline.append(lChannel);
		else
			lOffline.append(lChannel);
	}

	std::function<void (ChannelObjectWidget*)> lCallback = [this] (ChannelObjectWidget * pNewWidget)
	{
		connect(pNewWidget, &ChannelObjectWidget::clicked, this, &MainWindowFollowing::selected);
		connect(pNewWidget, &ChannelObjectWidget::onlineChanged, this, &MainWindowFollowing::onChannelsOnlineChanged);
	};

	addToFlowLayout(lOnline, mOnlineWidgets, ui->grpOnline->layout(), this, lCallback);
	addToFlowLayout(lOffline, mOfflineWidgets, ui->grpOffline->layout(), this, lCallback);
}

void MainWindowFollowing::onChannelsOnlineChanged()
{
	onChannelsUpdated();
}

} // namespace forms
