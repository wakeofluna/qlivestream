#include "config.h"
#include "main_window.h"
#include "ui_main_window.h"
#include "core/flowing_layout.h"
#include "core/network_access.h"
#include "core/profile.h"
#include "forms/category_object_widget.h"
#include "forms/debug_network_messages.h"
#include "forms/main_about.h"
#include "forms/select_profile.h"

#include <QApplication>
#include <QGridLayout>
#include <QLayoutItem>
#include <QCloseEvent>

namespace forms
{

MainWindow::MainWindow(Profile::UPtr && pProfile, QWidget *parent) : QMainWindow(parent, Qt::Window), mProfile(std::move(pProfile))
{
	ui = new Ui::MainWindow();
	ui->setupUi(this);

	ui->grpFollowingOnline->setLayout(new FlowingLayout(ui->grpFollowingOnline));
	ui->grpFollowingOffline->setLayout(new FlowingLayout(ui->grpFollowingOffline));
	ui->grpGamesFavourite->setLayout(new FlowingLayout(ui->grpGamesFavourite));
	ui->grpGamesAll->setLayout(new FlowingLayout(ui->grpGamesAll));

	ui->tabWidget->setCurrentWidget(ui->tabFollowing);
	ui->txtActiveAccount->setText(mProfile->account());
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::refreshFollowing()
{
}

void MainWindow::rollupFollowing()
{
}

void MainWindow::refreshGames()
{
	ui->statusbar->showMessage(tr("Fetching top category list ..."));
	mProfile->getTopCategories(0, 25, [this] (QVector<CategoryObject*> const& pList)
	{
		FlowingLayout * gridFavourite = static_cast<FlowingLayout*>(ui->grpGamesFavourite->layout());
		FlowingLayout * gridAll = static_cast<FlowingLayout*>(ui->grpGamesAll->layout());

		gridFavourite->clear(true);
		gridAll->clear(true);

		for (int i = 0; i < pList.size(); ++i)
		{
			CategoryObjectWidget * lWidget = new CategoryObjectWidget(pList[i], ui->tabGames);

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

		ui->statusbar->clearMessage();
	});
}

void MainWindow::rollupGames()
{
}

void MainWindow::closeEvent(QCloseEvent * event)
{
	QMainWindow::closeEvent(event);
	if (event->isAccepted())
	{
		auto * lWindow = NetworkAccess::networkCaptureWindow();
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

void MainWindow::on_mnuHelpDebugNetwork_triggered()
{
	auto * lWindow = NetworkAccess::networkCaptureWindow();
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

void MainWindow::on_btnFollowingRefresh_clicked()
{
	refreshFollowing();
}

void MainWindow::on_btnGamesRefresh_clicked()
{
	refreshGames();
}

} // namespace forms
