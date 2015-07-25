#include "config.h"
#include "category_channels.h"
#include "ui_category_channels.h"
#include "core/category_object.h"
#include "core/channel_object.h"
#include "core/profile.h"
#include "forms/channel_object_widget.h"
#include "forms/flowing_layout.h"

#include <QScrollBar>

namespace forms
{

CategoryChannels::CategoryChannels(CategoryObject & pCategory, QWidget * parent) : QWidget(parent), mCategory(pCategory)
{
	ui = new Ui::CategoryChannels();
	ui->setupUi(this);
	ui->scrAreaContents->setLayout(new FlowingLayout(ui->scrAreaContents));

	setWindowTitle(mCategory.name());

	connect(ui->btnRefresh, &QPushButton::clicked, this, &CategoryChannels::refresh);
	connect(ui->scrArea->verticalScrollBar(), &QScrollBar::valueChanged, this, &CategoryChannels::checkRollup);

	mCanRollup = false;
	refresh();
}

CategoryChannels::~CategoryChannels()
{
	delete ui;
}

void CategoryChannels::refresh()
{
	clear();
	rollup();
}

void CategoryChannels::rollup()
{
	Logger::StatusMessage lMessage("Fetching followed channels list ..");

	mCanRollup = false;
	mCategory.profile().getCategoryChannels(&mCategory, qMax(0, mChannels.count() - 5), 25, [this, lMessage] (QList<ChannelObject*> && pList)
	{
		mCanRollup = pList.count() >= 25;
		addData(std::move(pList));
	});
}

void CategoryChannels::checkRollup(int pSliderValue)
{
	bool lRollup = true;
	lRollup &= mCanRollup;
	lRollup &= (pSliderValue + 200 >= ui->scrArea->verticalScrollBar()->maximum());

	if (lRollup)
		rollup();
}

void CategoryChannels::clear()
{
	qDeleteAll(mChannels);
	mChannels.clear();

	qDeleteAll(mWidgets);
	mWidgets.clear();
}

void CategoryChannels::addData(QList<ChannelObject*> && pChannels)
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
		QLayout * lLayout = ui->scrAreaContents->layout();

		bool lFound = false;
		for (ChannelObjectWidget * lWidget : mWidgets)
			lFound |= lWidget->object() == lObject;

		if (!lFound)
		{
			ChannelObjectWidget * lWidget = new ChannelObjectWidget(lObject, this);
			mWidgets.append(lWidget);

			connect(lWidget, &ChannelObjectWidget::clicked, this, &CategoryChannels::selected);
			lLayout->addWidget(lWidget);

			if (lWidget->object()->logoUrl().isValid())
				accessCache
				(
						lWidget->object()->logoCacheString(),
						[this,lWidget] (CacheHitCallback && pCallback)
						{
							mCategory.profile().downloadLogo(lWidget->object()->logoUrl(), std::move(pCallback));
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
