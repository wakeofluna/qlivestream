#include "config.h"
#include "select_profile.h"
#include "ui_select_profile.h"
#include "core/profile.h"
#include "edit_profile.h"

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
}

SelectProfile::~SelectProfile()
{
}

void SelectProfile::updateProfiles()
{
	QStringList lProfiles = Profile::listProfiles();

	ui->cbbProfile->clear();
	ui->cbbProfile->addItems(lProfiles);
	ui->cbbProfile->addItem("[add new]");
}

void SelectProfile::on_btnEdit_clicked()
{
	Profile lProfile;

	int lIndex = ui->cbbProfile->currentIndex();
	if (lIndex < ui->cbbProfile->count() - 1)
	{
		lProfile = Profile::load(ui->cbbProfile->itemText(lIndex));
		if (!lProfile.isValid())
		{
			QMessageBox::critical(this, tr("Error in configuration"), tr("Failed to load profile"));
			return;
		}
	}

	EditProfile * lEditProfile = new EditProfile(lProfile, this);

	QDialog::DialogCode lResult = (QDialog::DialogCode) lEditProfile->exec();
	if (lResult == QDialog::Accepted)
		updateProfiles();
}

void SelectProfile::on_btnBox_accepted()
{
}

} // namespace forms
