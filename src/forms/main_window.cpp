#include "main_window.h"
#include "ui_main_window.h"

#include <algorithm>
#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QLabel>
#include <QList>
#include <QMessageBox>
#include <QObject>
#include <QTabWidget>
#include "category_channels.h"
#include "main_about.h"
#include "main_window_categories.h"
#include "main_window_following.h"
#include "select_profile.h"

#include "../core/i_category.h"
#include "../core/i_channel.h"
#include "../core/i_profile.h"
#include "../core/i_user.h"
#include "../core/logger.h"

namespace forms
{

MainWindow::MainWindow(std::unique_ptr<IProfile> && pProfile, QWidget *parent) : QMainWindow(parent, Qt::Window), mProfile(std::move(pProfile))
{
	mCategories = nullptr;
	mFollowing = nullptr;
	mYourChannel = nullptr;

	ui = new Ui::MainWindow();
	ui->setupUi(this);
	ui->txtActiveAccount->setText(mProfile->account());

	Logger::get()->pushStatusBar(ui->statusbar);

	switch (mProfile->level())
	{
		case IProfile::ANONYMOUS:
			ui->mnuViewChannel->setEnabled(false);
			break;

		case IProfile::VIEWER:
		case IProfile::EDITOR:
			break;

		case IProfile::STREAMER:
			ui->mnuViewChannel->trigger();
			break;
	}
}

MainWindow::~MainWindow()
{
	qDeleteAll(mChannels);
	delete mYourChannel;
	delete mFollowing;
	delete mCategories;
	delete ui;
}

void MainWindow::openCategoryTab(ICategory * pCategory)
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

	CategoryChannels * lCatChannels = new CategoryChannels(*pCategory, this);
	connect(lCatChannels, &CategoryChannels::selected, this, &MainWindow::openChannelTab);
	ui->tabWidget->addTab(lCatChannels, lCatChannels->windowTitle());
	ui->tabWidget->setCurrentWidget(lCatChannels);
}

void MainWindow::openChannelTab(IChannel * pChannel)
{
	QString lTitle = pChannel->owner().name();

	for (int i = 0; i < ui->tabWidget->count(); ++i)
	{
		if (ui->tabWidget->tabText(i) == lTitle)
		{
			ui->tabWidget->setCurrentIndex(i);
			return;
		}
	}

	QWidget * lWidget = new ChannelInfo(*pChannel, this);
	ui->tabWidget->addTab(lWidget, lWidget->windowTitle());
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
	if (mYourChannel == nullptr)
	{
		IChannel * lChannel = mProfile->self().channel(true);
		if (lChannel != nullptr)
		{
			mYourChannel = new ChannelInfo(*lChannel, this);
			ui->tabWidget->addTab(mYourChannel, mYourChannel->windowTitle());
			ui->tabWidget->setCurrentWidget(mYourChannel);
		}
		else
		{
			ui->mnuViewChannel->setChecked(false);
			QMessageBox::critical(this, "Open channel", QString("Unable to open channel: %1").arg(mProfile->lastError()));
		}
	}
	else
	{
		delete mYourChannel;
		mYourChannel = nullptr;
	}
}

void MainWindow::on_mnuViewFollowing_triggered()
{
	if (mFollowing == nullptr)
	{
		mFollowing = new MainWindowFollowing(*mProfile, this);
		connect(mFollowing, &MainWindowFollowing::selected, this, &MainWindow::openChannelTab);
		ui->tabWidget->addTab(mFollowing, mFollowing->windowTitle());
		ui->tabWidget->setCurrentWidget(mFollowing);
	}
	else
	{
		delete mFollowing;
		mFollowing = nullptr;
	}
}

void MainWindow::on_mnuViewCategories_triggered()
{
	if (mCategories == nullptr)
	{
		mCategories = new MainWindowCategories(*mProfile, this);
		connect(mCategories, &MainWindowCategories::selected, this, &MainWindow::openCategoryTab);
		ui->tabWidget->addTab(mCategories, mCategories->windowTitle());
		ui->tabWidget->setCurrentWidget(mCategories);
	}
	else
	{
		delete mCategories;
		mCategories = nullptr;
	}
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

	if (pWidget == mFollowing)
	{
		mFollowing = nullptr;
		ui->mnuViewFollowing->setChecked(false);
	}
	else if (pWidget == mCategories)
	{
		mCategories = nullptr;
		ui->mnuViewCategories->setChecked(false);
	}
	else if (pWidget == mYourChannel)
	{
		mYourChannel = nullptr;
		ui->mnuViewChannel->setChecked(false);
	}

	delete pWidget;
}

} // namespace forms
