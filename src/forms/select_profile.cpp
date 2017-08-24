#include "config.h"
#include "select_profile.h"
#include "ui_select_profile.h"
#include "edit_profile.h"
#include "perform_login.h"
#include "main_window.h"
#include "core/i_profile.h"

#include <algorithm>
#include <QString>
#include <QStringList>

namespace forms
{

SelectProfile::SelectProfile(QWidget * parent) : QWidget(parent, Qt::Window)
{
	ui = new Ui::SelectProfile();
	ui->setupUi(this);

	updateProfiles();

	auto lSelected = std::max_element(mProfiles.begin(), mProfiles.end());
	if (lSelected != mProfiles.end())
		setSelected(*lSelected);
}

SelectProfile::~SelectProfile()
{
}

bool SelectProfile::setSelected(ConfigProfile const & pProfile)
{
	QString lTag = pProfile.toString();
	for (int i = 0; i < ui->cbbProfile->count(); ++i)
	{
		if (ui->cbbProfile->itemText(i) == lTag)
		{
			ui->cbbProfile->setCurrentIndex(i);
			return true;
		}
	}

	return false;
}

void SelectProfile::updateProfiles()
{
	mProfiles = ConfigProfile::listProfiles();

	ui->cbbProfile->clear();
	for (auto & lProfile : mProfiles)
		ui->cbbProfile->addItem(lProfile.toString());
	ui->cbbProfile->addItem(tr("[add new]"));
}

void SelectProfile::on_btnEdit_clicked()
{
	ConfigProfile lProfile;
	if (ui->cbbProfile->currentIndex() < mProfiles.size())
		lProfile = mProfiles[ui->cbbProfile->currentIndex()];

	EditProfile * lEditProfile = new EditProfile(lProfile, this);
	lEditProfile->setAttribute(Qt::WA_DeleteOnClose);

	int lResult = lEditProfile->exec();
	if (lResult == QDialog::Accepted)
	{
		updateProfiles();
		setSelected(lProfile);
	}
}

void SelectProfile::on_btnBox_accepted()
{
	if (ui->cbbProfile->currentIndex() >= mProfiles.size())
	{
		on_btnEdit_clicked();
		return;
	}

	ConfigProfile & lConfig = mProfiles[ui->cbbProfile->currentIndex()];
	std::unique_ptr<IProfile> lProfile = lConfig.load();

	PerformLogin * lPerformLogin = new PerformLogin(*lProfile, this);
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

} // namespace forms
