#include "config.h"
#include "main_window.h"
#include "ui_main_window.h"
#include "core/network_access.h"
#include "core/profile.h"
#include "forms/category_object_widget.h"
#include "forms/debug_network_messages.h"
#include "forms/flowing_layout.h"
#include "forms/main_about.h"
#include "forms/select_profile.h"

#include <QApplication>
#include <QCloseEvent>
#include <QDebug>
#include <QGridLayout>
#include <QLayoutItem>
#include <QScrollArea>
#include <QScrollBar>

namespace forms
{

MainWindow::MainWindow(Profile::UPtr && pProfile, QWidget *parent) : QMainWindow(parent, Qt::Window), mProfile(std::move(pProfile))
{
	mCanRollupFollowing = false;
	mCanRollupGames = false;
	mRollupFollowingOffset = 0;
	mRollupGamesOffset = 0;

	ui = new Ui::MainWindow();
	ui->setupUi(this);

	ui->grpFollowingOnline->setLayout(new FlowingLayout(ui->grpFollowingOnline));
	ui->grpFollowingOffline->setLayout(new FlowingLayout(ui->grpFollowingOffline));
	ui->grpGamesFavourite->setLayout(new FlowingLayout(ui->grpGamesFavourite));
	ui->grpGamesAll->setLayout(new FlowingLayout(ui->grpGamesAll));

	connect(ui->scrFollowing->verticalScrollBar(), &QScrollBar::valueChanged, this, &MainWindow::checkRollupFollowing);
	connect(ui->scrGames->verticalScrollBar(), &QScrollBar::valueChanged, this, &MainWindow::checkRollupGames);

	ui->txtActiveAccount->setText(mProfile->account());

	// Make backups of the tab titles
	for (int i = 0; i < ui->tabWidget->count(); ++i)
	{
		QWidget * lWidget = ui->tabWidget->widget(i);
		lWidget->setProperty("title", ui->tabWidget->tabText(i));
	}

	switch (mProfile->level())
	{
		case Profile::ANONYMOUS:
			ui->mnuViewChannel->setEnabled(false);
			// no break

		case Profile::VIEWER:
		case Profile::MODERATOR:
			toggleTabVisible(ui->tabYourChannel);
			ui->mnuViewChannel->setChecked(false);
			ui->tabWidget->setCurrentWidget(ui->tabFollowing);
			break;

		case Profile::STREAMER:
			ui->tabWidget->setCurrentWidget(ui->tabYourChannel);
			break;
	}
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::refreshFollowing()
{
	mRollupFollowingOffset = 0;
	rollupFollowing();
}

void MainWindow::rollupFollowing()
{
}

void MainWindow::refreshGames()
{
	mRollupGamesOffset = 0;
	rollupGames();
}

void MainWindow::rollupGames()
{
	mCanRollupGames = false;

	ui->statusbar->showMessage(tr("Fetching top category list ..."));
	mProfile->getTopCategories(mRollupGamesOffset, 25, [this] (QVector<CategoryObject*> const& pList)
	{
		appendCategoryObjects(pList, mRollupGamesOffset == 0);
		ui->statusbar->clearMessage();

		mRollupGamesOffset += pList.size();
		mCanRollupGames = (pList.size() == 25);
	});
}

void MainWindow::openTabFor(CategoryObject * pCategory)
{
	QString lTitle = pCategory->name();

	for (int i = 0; i < ui->tabWidget->count(); ++i)
	{
		if (ui->tabWidget->tabText(i) == lTitle)
		{
			ui->tabWidget->setCurrentIndex(i);
			return;
		}
	}

	QWidget * lWidget = new QWidget(this);
	ui->tabWidget->addTab(lWidget, lTitle);
	ui->tabWidget->setCurrentWidget(lWidget);
}

void MainWindow::closeEvent(QCloseEvent * event)
{
	QMainWindow::closeEvent(event);
	if (event->isAccepted())
	{
		QWidget * lWindow = Logger::get()->networkCaptureWindow();
		lWindow->close();
	}
}

void MainWindow::on_mnuFileLogout_triggered()
{
	SelectProfile * lSelect = new SelectProfile();
	lSelect->setAttribute(Qt::WA_DeleteOnClose);
	lSelect->show();
	close();
}

void MainWindow::on_mnuFileExit_triggered()
{
	QApplication::exit(0);
}

void MainWindow::on_mnuViewChannel_triggered()
{
	toggleTabVisible(ui->tabYourChannel);
}

void MainWindow::on_mnuViewFollowing_triggered()
{
	if (toggleTabVisible(ui->tabFollowing))
		refreshFollowing();
}

void MainWindow::on_mnuViewGames_triggered()
{
	if (toggleTabVisible(ui->tabGames))
		refreshGames();
}

void MainWindow::on_mnuHelpDebugNetwork_triggered()
{
	QWidget * lWindow = Logger::get()->networkCaptureWindow();
	lWindow->show();
}

void MainWindow::on_mnuHelpAbout_triggered()
{
	MainAbout * lAbout = new MainAbout(this);
	lAbout->setAttribute(Qt::WA_DeleteOnClose);
	lAbout->show();
}

void MainWindow::on_mnuHelpAboutQt_triggered()
{
	QApplication::aboutQt();
}

void MainWindow::on_tabWidget_tabCloseRequested(int pIndex)
{
	QWidget * pWidget = ui->tabWidget->widget(pIndex);
	ui->tabWidget->removeTab(pIndex);

	if (pWidget == ui->tabYourChannel)
		ui->mnuViewChannel->setChecked(false);
	else if (pWidget == ui->tabFollowing)
		ui->mnuViewFollowing->setChecked(false);
	else if (pWidget == ui->tabGames)
		ui->mnuViewGames->setChecked(false);
	else
		delete pWidget;
}

void MainWindow::on_btnFollowingRefresh_clicked()
{
	refreshFollowing();
}

void MainWindow::on_btnGamesRefresh_clicked()
{
	refreshGames();
}

bool MainWindow::toggleTabVisible(QWidget * pWidget)
{
	int i = ui->tabWidget->indexOf(pWidget);
	if (i >= 0)
	{
		ui->tabWidget->removeTab(i);
		return false;
	}
	else
	{
		ui->tabWidget->addTab(pWidget, pWidget->property("title").toString());
		ui->tabWidget->setCurrentWidget(pWidget);
		return true;
	}
}

void MainWindow::checkRollupFollowing(int pSliderValue)
{
	if (mCanRollupFollowing && checkRollupFor(ui->scrFollowing, pSliderValue))
		rollupFollowing();
}

void MainWindow::checkRollupGames(int pSliderValue)
{
	if (mCanRollupGames && checkRollupFor(ui->scrGames, pSliderValue))
		rollupGames();
}

bool MainWindow::checkRollupFor(QScrollArea * pArea, int pSliderValue)
{
	QScrollBar * lBar = pArea->verticalScrollBar();
	int lMaximum = lBar->maximum();

	return (pSliderValue + 200 >= lMaximum);
}

void MainWindow::appendCategoryObjects(QVector<CategoryObject*> const& pList, bool pClear)
{
	FlowingLayout * gridFavourite = static_cast<FlowingLayout*>(ui->grpGamesFavourite->layout());
	FlowingLayout * gridAll = static_cast<FlowingLayout*>(ui->grpGamesAll->layout());

	if (pClear)
	{
		gridFavourite->clear(true);
		gridAll->clear(true);
	}

	for (int i = 0; i < pList.size(); ++i)
	{
		CategoryObjectWidget * lWidget = new CategoryObjectWidget(pList[i], ui->tabGames);
		connect(lWidget, &CategoryObjectWidget::clicked, this, &MainWindow::openTabFor);

		FlowingLayout * lTarget = lWidget->object()->followed() ? gridFavourite : gridAll;
		lTarget->addWidget(lWidget);

		QString lLogoCacheString = lWidget->object()->logoCacheString();
		accessCache(lLogoCacheString,
			[this,lWidget] (CacheHitCallback && pCallback)
			{
				mProfile->downloadLogo(lWidget->object()->logoUrl(), std::move(pCallback));
			},
			[lWidget] (QByteArray const& pData)
			{
				lWidget->setLogo(pData);
			}
		);
	}
}


} // namespace forms
