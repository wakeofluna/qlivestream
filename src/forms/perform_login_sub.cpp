#include "config.h"
#include "perform_login_sub.h"
#include "ui_perform_login_acquire.h"
#include "ui_perform_login_error.h"
#include "ui_perform_login_invalid.h"
#include "core/profile.h"

#include <QAbstractButton>
#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QStyle>
#include <QUrl>

namespace forms
{

SubPanelAcquire::SubPanelAcquire(Profile & pProfile, QWidget * parent) : QWidget(parent), mProfile(pProfile)
{
	ui = new Ui::PerformLoginAcquire();
	ui->setupUi(this);
}

SubPanelAcquire::~SubPanelAcquire()
{
	delete ui;
}

void SubPanelAcquire::on_btnAcquire_clicked()
{
	QUrl lUrl = mProfile.acquireTokenUrl();
	if (!QDesktopServices::openUrl(lUrl))
	{
		QString lFallback = lUrl.toString();

		ui->txtFallback->setText(lFallback);
		ui->stkMethod->setCurrentIndex(1);

		QClipboard * lClipboard = QApplication::clipboard();
		lClipboard->setText(lFallback);
	}

	ui->txtToken->setFocus();
	ui->btnToken->setDefault(true);
}

void SubPanelAcquire::on_txtToken_textChanged(QString const & pString)
{
	ui->btnToken->setEnabled(!ui->txtToken->text().isEmpty());
}

void SubPanelAcquire::on_btnToken_clicked()
{
	emit onSetToken(ui->txtToken->text());
}


SubPanelError::SubPanelError(QString pError, QWidget * parent) : QWidget(parent)
{
	ui = new Ui::PerformLoginError();
	ui->setupUi(this);
	ui->lblIcon->setPixmap(QApplication::style()->standardPixmap(QStyle::SP_MessageBoxCritical));
	ui->lblError->setText(pError);
}

SubPanelError::~SubPanelError()
{
	delete ui;
}

void SubPanelError::on_btnError_rejected()
{
	emit cancel();
}

SubPanelInvalid::SubPanelInvalid(Profile & pProfile, QWidget * parent) : QWidget(parent), mProfile(pProfile)
{
	ui = new Ui::PerformLoginInvalid();
	ui->setupUi(this);
	ui->lblIcon->setPixmap(QApplication::style()->standardPixmap(QStyle::SP_MessageBoxCritical));
}

SubPanelInvalid::~SubPanelInvalid()
{
	delete ui;
}

void SubPanelInvalid::on_btnInvalid_accepted()
{
	emit retry();
}

void SubPanelInvalid::on_btnInvalid_rejected()
{
	emit cancel();
}


} // namespace forms
