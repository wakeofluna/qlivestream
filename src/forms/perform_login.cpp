#include "config.h"
#include "perform_login.h"
#include "perform_login_sub.h"

#include "../core/i_profile.h"
#include "ui_perform_login.h"

#include <QTimer>

namespace forms
{

PerformLogin::PerformLogin(IProfile & pProfile, QWidget * parent) : QDialog(parent, Qt::Window), mProfile(pProfile)
{
	ui = new Ui::PerformLogin();
	ui->setupUi(this);

	mSubPanel = nullptr;
	mStep = -1;
}

PerformLogin::~PerformLogin()
{
	delete ui;
}

void PerformLogin::showEvent(QShowEvent * event)
{
	if (mStep == -1)
		proceed();
}

void PerformLogin::proceed()
{
	++mStep;
	runStep();
}

void PerformLogin::restart()
{
	mStep = 0;
	runStep();
}

void PerformLogin::runStep()
{
	QTimer::singleShot(0, [this] () { runStepImpl(); });
}

void PerformLogin::runStepImpl()
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
			ui->prgStep->setFormat(tr("Login to service"));
			if (mProfile.token().isEmpty() && mProfile.level() != IProfile::ANONYMOUS)
			{
				SubPanelAcquire * lPanel = new SubPanelAcquire(mProfile, this);
				connect(lPanel, &SubPanelAcquire::onSetToken, [this] (QString pToken)
				{
					mProfile.updateToken(pToken);
					proceed();
				});
				mSubPanel = lPanel;
				ui->layout->addWidget(mSubPanel);
			}
			else
			{
				proceed();
			}
			break;

		case 1:
		{
			ui->prgStep->setValue(50);
			ui->prgStep->setFormat(tr("Checking login"));
			mProfile.performLogin([this] ()
			{
				if (mProfile.hasError())
				{
					SubPanelError * lPanel = new SubPanelError(mProfile.lastError(), this);
					connect(lPanel, &SubPanelError::cancel, [this]
					{
						reject();
					});
					mSubPanel = lPanel;
					ui->layout->addWidget(mSubPanel);
				}
				else
				{
					proceed();
				}
			});
			break;
		}

		case 2:
			if (!mProfile.loggedIn())
			{
				SubPanelInvalid * lPanel = new SubPanelInvalid(mProfile, this);
				connect(lPanel, &SubPanelInvalid::cancel, [this]
				{
					reject();
				});
				connect(lPanel, &SubPanelInvalid::retry, [this]
				{
					mProfile.updateToken(QString());
					restart();
				});
				mSubPanel = lPanel;
				ui->layout->addWidget(mSubPanel);
			}
			else
			{
				proceed();
			}
			break;

		case 3:
			ui->prgStep->setValue(75);
			ui->prgStep->setFormat("Getting channel information");
			mProfile.performPostLogin([this] ()
			{
				proceed();
			});
			break;

		case 4:
			accept();
			break;

		default:
			break;

	}
}

} // namespace forms
