#include "config.h"
#include "main_about.h"
#include "ui_main_about.h"

namespace forms
{

MainAbout::MainAbout(QWidget *parent) : QWidget(parent, Qt::Window)
{
	ui = new Ui::MainAbout();
	ui->setupUi(this);

	connect(ui->btnOk, &QDialogButtonBox::accepted, this, &MainAbout::close);
}

MainAbout::~MainAbout()
{
	delete ui;
}


} // namespace forms
