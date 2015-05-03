#include "config.h"
#include "perform_login.h"
#include "ui_perform_login.h"
#include "core/profile.h"

#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QUrl>

namespace forms
{

PerformLogin::PerformLogin(Profile & pProfile, QWidget * parent) : QDialog(parent, Qt::Window), mProfile(pProfile)
{
	ui = new Ui::PerformLogin();
	ui->setupUi(this);
}

PerformLogin::~PerformLogin()
{
	delete ui;
}

void PerformLogin::on_btnAcquire_clicked()
{
	QUrl lUrl;
	lUrl.setScheme("https");
	lUrl.setHost("kitsune.astralkey.nl");
	lUrl.setPath("/qtwitch");
	lUrl.setFragment(QString("scope=") + mProfile.mRequested.toString());

	if (!QDesktopServices::openUrl(lUrl))
	{
		QString lFallback = lUrl.toString();

		ui->txtTokenFallback->setText(lFallback);
		ui->stkTokenFallback->setCurrentIndex(1);

		QClipboard * lClipboard = QApplication::clipboard();
		lClipboard->setText(lFallback);
	}

	ui->txtToken->setFocus();
	ui->btnToken->setDefault(true);
}

void PerformLogin::on_txtToken_textChanged(QString const & pValue)
{
	ui->btnToken->setDisabled(ui->txtToken->text().isEmpty());
}

void PerformLogin::on_btnToken_clicked()
{
	// TODO goto next step in verification
	accept();
}

} // namespace forms
