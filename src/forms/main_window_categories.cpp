#include "config.h"
#include "main_window_categories.h"
#include "ui_main_window_categories.h"
#include "forms/flowing_layout.h"
#include "forms/category_object_widget.h"
#include "core/profile.h"

#include <algorithm>
#include <QScrollBar>

namespace forms
{

MainWindowCategories::MainWindowCategories(Profile & pProfile, QWidget * parent) : QWidget(parent), mProfile(pProfile)
{
	ui = new Ui::MainWindowCategories();
	ui->setupUi(this);

	ui->grpFavourite->setLayout(new FlowingLayout(ui->grpFavourite));
	ui->grpAll->setLayout(new FlowingLayout(ui->grpAll));

	connect(ui->btnRefresh, &QPushButton::clicked, this, &MainWindowCategories::refresh);
	connect(ui->scrArea->verticalScrollBar(), &QScrollBar::valueChanged, this, &MainWindowCategories::checkRollup);

	mCanRollup = false;
}

MainWindowCategories::~MainWindowCategories()
{
	qDeleteAll(mTop);
	qDeleteAll(mFavourite);
	qDeleteAll(mCategories);
	delete ui;
}

void MainWindowCategories::refresh()
{
	clear();
	rollup();
}

void MainWindowCategories::rollup()
{
	Logger::StatusMessage lMessage("Fetching top categories list ..");

	mCanRollup = false;
	mProfile.getTopCategories(qMax(0, mTop.count() - 5), 25, [this, lMessage] (QList<CategoryObject*> && pList)
	{
		mCanRollup = pList.count() >= 25;
		addData(std::move(pList));
	});
}

void MainWindowCategories::checkRollup(int pSliderValue)
{
	bool lRollup = true;
	lRollup &= mCanRollup;
	lRollup &= (pSliderValue + 200 >= ui->scrArea->verticalScrollBar()->maximum());

	if (lRollup)
		rollup();
}

void MainWindowCategories::clear()
{
	qDeleteAll(mTop);
	mTop.clear();

	for (CategoryObject *& pObject : mCategories)
	{
		if (pObject->followed())
		{
			pObject->clear();
		}
		else
		{
			delete pObject;
			pObject = nullptr;
		}
	};

	auto lEnd = std::remove(mCategories.begin(), mCategories.end(), nullptr);
	mCategories.erase(lEnd, mCategories.end());
}

void MainWindowCategories::addData(QList<CategoryObject*> && pCategories)
{
	for (CategoryObject * lNew : pCategories)
	{
		bool lFound = false;
		for (CategoryObject * lExisting : mCategories)
			lFound |= lExisting->updateFrom(*lNew);

		if (lFound)
			delete lNew;
		else
			mCategories.append(lNew);
	}

	pCategories.clear();

	for (CategoryObject * lObject : mCategories)
	{
		bool lFound = false;
		for (CategoryObjectWidget * lWidget : mTop)
			lFound |= lWidget->object() == lObject;

		if (!lFound)
		{
			CategoryObjectWidget * lWidget = new CategoryObjectWidget(lObject, this);
			mTop.append(lWidget);

			connect(lWidget, &CategoryObjectWidget::clicked, this, &MainWindowCategories::selected);
			ui->grpAll->layout()->addWidget(lWidget);

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
