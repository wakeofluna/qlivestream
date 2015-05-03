#include "config.h"
#include "main_window.h"
#include "ui_main_window.h"
#include "core/profile.h"

namespace forms
{

MainWindow::MainWindow(Profile pProfile, QWidget *parent) : QMainWindow(parent, Qt::Window), mProfile(pProfile)
{
	ui = new Ui::MainWindow();
	ui->setupUi(this);
}

MainWindow::~MainWindow()
{
	delete ui;
}

} // namespace forms
