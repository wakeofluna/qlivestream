#include "config.h"
#include "main_window.h"
#include "ui_main_window.h"
#include "core/profile.h"
#include "forms/main_about.h"
#include "forms/select_profile.h"

#include <QApplication>

namespace forms
{

MainWindow::MainWindow(Profile::UPtr && pProfile, QWidget *parent) : QMainWindow(parent, Qt::Window), mProfile(std::move(pProfile))
{
	ui = new Ui::MainWindow();
	ui->setupUi(this);
	ui->tabWidget->setCurrentWidget(ui->tabFollowing);
	ui->txtActiveAccount->setText(mProfile->account());
}

MainWindow::~MainWindow()
{
	delete ui;
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


} // namespace forms
