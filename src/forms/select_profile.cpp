#include "config.h"
#include "select_profile.h"
#include "ui_select_profile.h"
#include "core/profile.h"
#include "edit_profile.h"
#include "perform_login.h"
#include "main_window.h"

#include <QComboBox>
#include <QMessageBox>
#include <QSettings>
#include <QString>
#include <QStringList>

namespace forms
{

SelectProfile::SelectProfile(QWidget * parent) : QWidget(parent, Qt::Window)
{
	ui = new Ui::SelectProfile();
	ui->setupUi(this);

	updateProfiles();

	//SETTINGS(settings);
	//settings.beginGroup(objectName());
	//setSelected(settings.value("last_selected").toString());
}

SelectProfile::~SelectProfile()
{
}

bool SelectProfile::setSelected(QString pName)
{
	for (int i = 0; i < ui->cbbProfile->count(); ++i)
	{
		if (ui->cbbProfile->itemText(i) == pName)
		{
			ui->cbbProfile->setCurrentIndex(i);
			return true;
		}
	}

	return false;
}

void SelectProfile::updateProfiles()
{
	QStringList lProfiles = Profile::listProfiles();

	ui->cbbProfile->clear();
	ui->cbbProfile->addItems(lProfiles);
	ui->cbbProfile->addItem(tr("[add new]"));
}

void SelectProfile::on_btnEdit_clicked()
{
	Profile lProfile = loadSelectedProfile();

	EditProfile * lEditProfile = new EditProfile(lProfile, this);
	lEditProfile->setAttribute(Qt::WA_DeleteOnClose);

	int lResult = lEditProfile->exec();
	if (lResult == QDialog::Accepted)
	{
		QString lSelected = lProfile.mAccount;
		updateProfiles();
		setSelected(lSelected);
	}
}

void SelectProfile::on_btnBox_accepted()
{
	Profile lProfile = loadSelectedProfile();
	if (!lProfile.isValid())
	{
		on_btnEdit_clicked();
		return;
	}

	//SETTINGS(settings);
	//settings.beginGroup(objectName());
	//settings.setValue("last_selected", lProfile.mName);

	PerformLogin * lPerformLogin = new PerformLogin(lProfile, this);
	lPerformLogin->setAttribute(Qt::WA_DeleteOnClose);
	int lResult = lPerformLogin->exec();
	if (lResult == QDialog::Accepted)
	{
		MainWindow * lMainWindow = new MainWindow(std::move(lProfile));
		lMainWindow->setAttribute(Qt::WA_DeleteOnClose);
		lMainWindow->show();
		close();
	}
}

Profile SelectProfile::loadSelectedProfile()
{
	Profile lProfile;

	int lIndex = ui->cbbProfile->currentIndex();
	if (lIndex < ui->cbbProfile->count() - 1)
	{
		QString lName = ui->cbbProfile->itemText(lIndex);
		lProfile = Profile::load(lName);
		if (!lProfile.isValid())
			QMessageBox::critical(this, tr("Error in configuration"), tr("Failed to load profile"));
	}

	return lProfile;
}

} // namespace forms
