#include "config.h"
#include "main_window_categories.h"
#include "ui_main_window_categories.h"
#include "forms/flowing_layout.h"
#include "forms/category_object_widget.h"
#include "forms/form_helpers.h"
#include "misc.h"

#include <algorithm>
#include <QScrollBar>

#include "../core/i_category.h"
#include "../core/i_profile.h"

namespace forms
{

MainWindowCategories::MainWindowCategories(IProfile & pProfile, QWidget * parent) : QWidget(parent), mProfile(pProfile)
{
	ui = new Ui::MainWindowCategories();
	ui->setupUi(this);

	ui->grpFavourite->setLayout(new FlowingLayout(ui->grpFavourite, qobject_less<QWidget, CategoryObjectWidget>()));
	ui->grpAll->setLayout(new FlowingLayout(ui->grpAll, qobject_less<QWidget, CategoryObjectWidget>()));

	connect(ui->btnRefresh, &QPushButton::clicked, this, &MainWindowCategories::refresh);
	connect(ui->scrArea->verticalScrollBar(), &QScrollBar::valueChanged, this, &MainWindowCategories::checkRollup);
	connect(&mProfile, &IProfile::followedCategoriesUpdated, this, &MainWindowCategories::onCategoriesUpdated);
	connect(&mProfile, &IProfile::topCategoriesUpdated, this, &MainWindowCategories::onCategoriesUpdated);

	mProfile.resetFollowedCategories();
	mProfile.resetTopCategories();
}

MainWindowCategories::~MainWindowCategories()
{
	qDeleteAll(mTop);
	qDeleteAll(mFavourite);
	delete ui;
}

void MainWindowCategories::refresh()
{
	mProfile.resetTopCategories();
}

void MainWindowCategories::rollup()
{
	mProfile.rollupTopCategories();
}

void MainWindowCategories::checkRollup(int pSliderValue)
{
	if (pSliderValue + 200 >= ui->scrArea->verticalScrollBar()->maximum())
		rollup();
}

void MainWindowCategories::onCategoriesUpdated()
{
	addToFlowLayout(mProfile.followedCategories(), mFavourite, ui->grpFavourite->layout(), this, &MainWindowCategories::selected);
	addToFlowLayout(mProfile.topCategories(), mTop, ui->grpAll->layout(), this, &MainWindowCategories::selected);
}

} // namespace forms
