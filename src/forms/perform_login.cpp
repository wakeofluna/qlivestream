#include "config.h"
#include "perform_login.h"
#include "perform_login_sub.h"
#include "ui_perform_login.h"
#include "core/profile.h"
#include "replies/token_check.h"
#include <QMessageBox>

namespace forms
{

PerformLogin::PerformLogin(Profile & pProfile, QWidget * parent) : QDialog(parent, Qt::Window), mProfile(pProfile)
{
	ui = new Ui::PerformLogin();
	ui->setupUi(this);

	mSubPanel = nullptr;
	mStep = 0;
	runStep();
}

PerformLogin::~PerformLogin()
{
	delete ui;
}

void PerformLogin::proceed()
{
	++mStep;
	runStep();
}

void PerformLogin::restart()
{
	mStep = 0;
	mTokenCheck.reset();
	runStep();
}

void PerformLogin::runStep()
{
	if (mSubPanel)
	{
		ui->layout->removeWidget(mSubPanel);
		mSubPanel->deleteLater();
		mSubPanel = nullptr;
	}

	switch (mStep)
	{
		case 0:
			ui->prgStep->setValue(25);
			ui->prgStep->setFormat(tr("Getting permission from service"));
			if (mProfile.mAuthToken.isEmpty())
			{
				SubPanelAcquire * lPanel = new SubPanelAcquire(mProfile, this);
				mSubPanel = lPanel;
				connect(lPanel, &SubPanelAcquire::onSetToken, [this] (QString pToken)
				{
					mProfile.mAuthToken = pToken;
					mProfile.save();
					proceed();
				});
				break;
			}
			proceed();
			return;

		case 1:
		{
			ui->prgStep->setValue(50);
			ui->prgStep->setFormat(tr("Checking granted permissions"));
			TokenCheck::request(*this, mProfile, [this] (TokenCheck::Ptr & pToken)
			{
				if (pToken->hasError())
				{
					QMessageBox::critical(this, "Invalid reply", pToken->error());
					reject();
					return;
				}
				mTokenCheck.swap(pToken);
				proceed();
			});
			break;
		}

		case 2:
			if (!mTokenCheck->isValid())
			{
				//mProfile.mAuthToken.clear();
				//mProfile.save();
				//restart();
				break;
			}
			proceed();
			return;

		case 3:
			if (mTokenCheck->username() != mProfile.account())
			{
				//mProfile.mAuthToken.clear();
				//mProfile.save();
				//restart();

				//mProfile.mAccount = mTokenCheck->username();
				//mProfile.save();
				//proceed();

				break;
			}
			proceed();
			return;

		case 4:
			if (mTokenCheck->scopes() != mProfile.requested())
			{
				//mProfile.mAuthToken.clear();
				//mProfile.save();
				//restart();

				//mProfile.mRequested = mTokenCheck->scopes();
				//mProfile.save();
				//proceed();

				break;
			}
			proceed();
			return;

		case 5:
			ui->prgStep->setValue(75);
			ui->prgStep->setFormat("Getting channel information");
			proceed();
			return;

		case 6:
			accept();
			break;

		default:
			break;

	}

	if (mSubPanel != nullptr)
		ui->layout->addWidget(mSubPanel);

	QSize lHint = sizeHint();
	QSize lSize = size();
	setFixedSize(lSize.width(), lHint.height());
}

} // namespace forms
