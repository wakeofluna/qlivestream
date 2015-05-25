#include "config.h"
#include "main_window_following.h"
#include "ui_main_window_following.h"
#include "core/channel_object.h"
#include "core/profile.h"
#include "forms/channel_object_widget.h"
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
	refresh();
}

MainWindowFollowing::~MainWindowFollowing()
{
	qDeleteAll(mChannels);
	qDeleteAll(mWidgets);
	delete ui;
}

void MainWindowFollowing::refresh()
{
	clear();
	rollup();
}

void MainWindowFollowing::rollup()
{
	Logger::StatusMessage lMessage("Fetching followed channels list ..");

	mCanRollup = false;
	mProfile.getFollowedChannels(qMax(0, mChannels.count() - 5), 25, [this, lMessage] (QList<ChannelObject*> && pList)
	{
		mCanRollup = pList.count() >= 25;
		addData(std::move(pList));
	});
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
	qDeleteAll(mChannels);
	mChannels.clear();

	qDeleteAll(mWidgets);
	mWidgets.clear();
}

void MainWindowFollowing::addData(QList<ChannelObject*> && pChannels)
{
	for (ChannelObject * lNew : pChannels)
	{
		bool lFound = false;
		for (ChannelObject * lExisting : mChannels)
			lFound |= lExisting->updateFrom(*lNew);

		if (lFound)
			delete lNew;
		else
			mChannels.append(lNew);
	}

	pChannels.clear();

	for (ChannelObject * lObject : mChannels)
	{
		auto * lLayout = (lObject->isOnline() ? ui->grpOnline->layout() : ui->grpOffline->layout());

		bool lFound = false;
		for (ChannelObjectWidget * lWidget : mWidgets)
			lFound |= lWidget->object() == lObject;

		if (!lFound)
		{
			ChannelObjectWidget * lWidget = new ChannelObjectWidget(lObject, this);
			mWidgets.append(lWidget);

			connect(lWidget, &ChannelObjectWidget::clicked, this, &MainWindowFollowing::selected);
			lLayout->addWidget(lWidget);

			accessCache
			(
					lWidget->object()->logoCacheString(),
					[this,lWidget] (CacheHitCallback && pCallback)
					{
						mProfile.downloadLogo(lWidget->object()->logoUrl(), std::move(pCallback));
					},
					[lWidget] (QByteArray const & pData)
					{
						lWidget->setLogo(pData);
					}
			);
		}
	}
}

} // namespace forms
