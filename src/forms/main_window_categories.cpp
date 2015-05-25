#include "config.h"
#include "main_window_categories.h"
#include "ui_main_window_categories.h"
#include "core/category_object.h"
#include "core/profile.h"
#include "forms/flowing_layout.h"
#include "forms/category_object_widget.h"

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

	Logger::StatusMessage lMessage("Fetching followed categories list ..");
	mProfile.getFollowedCategories(0, 100, [this, lMessage] (QList<CategoryObject*> && pList)
	{
		addData(std::move(pList));
	});
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

	std::sort(mCategories.begin(), mCategories.end(), [] (CategoryObject const * lhs, CategoryObject const * rhs) -> bool
	{
		if (lhs->numViewers() != rhs->numViewers())
			return lhs->numViewers() > rhs->numViewers();
		if (lhs->numChannels() != rhs->numChannels())
			return lhs->numChannels() > rhs->numChannels();
		return lhs->name() < rhs->name();
	});

	for (CategoryObject * lObject : mCategories)
	{
		if (!lObject->followed())
			continue;

		addToList(true, lObject);
	}

	for (CategoryObject * lObject : mCategories)
	{
		if (!lObject->isValid())
			continue;

		addToList(false, lObject);
	}
}

void MainWindowCategories::addToList(bool pFavourite, CategoryObject * pCategory)
{
	auto & lList = (pFavourite ? mFavourite : mTop);
	auto * lLayout = (pFavourite ? ui->grpFavourite->layout() : ui->grpAll->layout());

	bool lFound = false;
	for (CategoryObjectWidget * lWidget : lList)
		lFound |= lWidget->object() == pCategory;

	if (!lFound)
	{
		CategoryObjectWidget * lWidget = new CategoryObjectWidget(pCategory, this);
		lList.append(lWidget);

		connect(lWidget, &CategoryObjectWidget::clicked, this, &MainWindowCategories::selected);
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

} // namespace forms
