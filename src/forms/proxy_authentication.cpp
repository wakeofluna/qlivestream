#include "config.h"
#include "proxy_authentication.h"
#include "ui_proxy_authentication.h"

namespace forms
{

ProxyAuthentication::ProxyAuthentication(QWidget * parent) : QDialog(parent, Qt::Dialog)
{
	ui = new Ui::ProxyAuthentication();
	ui->setupUi(this);

	setWindowIcon(style()->standardIcon(QStyle::SP_MessageBoxQuestion));
}

ProxyAuthentication::~ProxyAuthentication()
{
	delete ui;
}

void ProxyAuthentication::setUsername(QString pUsername)
{
	ui->txtUsername->setText(pUsername);
	if (!pUsername.isEmpty())
		ui->txtPassword->setFocus();
}

QString ProxyAuthentication::username() const
{
	return ui->txtUsername->text();
}

QString ProxyAuthentication::password() const
{
	return ui->txtPassword->text();
}

} // namespace forms
