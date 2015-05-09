#include "config.h"
#include "perform_login.h"
#include "ui_perform_login.h"
#include "core/profile.h"
#include "replies/token_check.h"

#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QMessageBox>
#include <QUrl>

namespace forms
{

PerformLogin::PerformLogin(Profile & pProfile, QWidget * parent) : QDialog(parent, Qt::Window), mProfile(pProfile)
{
	ui = new Ui::PerformLogin();
	ui->setupUi(this);

	mStep = 0;

	if (!mProfile.mAuthToken.isEmpty())
	{
		mStep = 1;
		ui->stkFeedback->setCurrentIndex(mStep);
	}

	on_stkFeedback_currentChanged(ui->stkFeedback->currentIndex());
}

PerformLogin::~PerformLogin()
{
	delete ui;
}

void PerformLogin::on_stkFeedback_currentChanged(int pIndex)
{
	switch (pIndex)
	{
		case 0:
		{
			ui->prgStep->setValue(0);
			ui->prgStep->setFormat("Acquiring token from Twitch");
			break;
		}

		case 1:
		{
			ui->prgStep->setValue(20);
			ui->prgStep->setFormat("Checking login token");
			TokenCheck::request(*this, mProfile, [this] (TokenCheck::Ptr & p) { replyTokenCheck(p); });
			break;
		}

		default:
			break;
	}
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
	mProfile.mAuthToken = ui->txtToken->text();
	mProfile.save();

	// TODO goto next step in verification
	accept();
}

void PerformLogin::replyTokenCheck(TokenCheck::Ptr & pToken)
{
	if (pToken->hasError())
	{
		QMessageBox::critical(this, "Parse error in response", "Got invalid reply, cannot continue");
		reject();
		return;
	}

	if (pToken->isValid())
		accept();
}

} // namespace forms
