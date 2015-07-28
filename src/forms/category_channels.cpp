#include "config.h"
#include "category_channels.h"
#include "ui_category_channels.h"
#include "forms/channel_object_widget.h"
#include "forms/flowing_layout.h"
#include "forms/form_helpers.h"

#include <QScrollBar>

#include "../core/i_category.h"
#include "../core/i_channel.h"
#include "../core/i_profile.h"

namespace forms
{

CategoryChannels::CategoryChannels(ICategory & pCategory, QWidget * parent) : QWidget(parent), mCategory(pCategory)
{
	ui = new Ui::CategoryChannels();
	ui->setupUi(this);
	ui->scrAreaContents->setLayout(new FlowingLayout(ui->scrAreaContents, qobject_less<QWidget, ChannelObjectWidget>()));

	setWindowTitle(mCategory.name());

	connect(ui->btnRefresh, &QPushButton::clicked, this, &CategoryChannels::refresh);
	connect(ui->scrArea->verticalScrollBar(), &QScrollBar::valueChanged, this, &CategoryChannels::checkRollup);
	connect(&mCategory, &ICategory::channelsUpdated, this, &CategoryChannels::onChannelsUpdated);

	mCategory.resetChannels();
}

CategoryChannels::~CategoryChannels()
{
	qDeleteAll(mWidgets);
	delete ui;
}

void CategoryChannels::refresh()
{
	mCategory.resetChannels();
}

void CategoryChannels::rollup()
{
	mCategory.rollupChannels();
}

void CategoryChannels::checkRollup(int pSliderValue)
{
	if (pSliderValue + 200 >= ui->scrArea->verticalScrollBar()->maximum())
		rollup();
}

void CategoryChannels::onChannelsUpdated()
{
	addToFlowLayout(category().channels(), mWidgets, ui->scrAreaContents->layout(), this, &CategoryChannels::selected);
}

} // namespace forms
