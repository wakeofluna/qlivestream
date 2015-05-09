#include "config.h"
#include "perform_login_sub.h"
#include "ui_perform_login_acquire.h"
#include "core/profile.h"

#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
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
	QUrl lUrl;
	lUrl.setScheme("https");
	lUrl.setHost("kitsune.astralkey.nl");
	lUrl.setPath("/qtwitch");
	lUrl.setFragment(QString("scope=") + mProfile.requested().toString());

	if (true)//!QDesktopServices::openUrl(lUrl))
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

} // namespace forms
