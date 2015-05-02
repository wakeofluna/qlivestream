#include "config.h"
#include "edit_profile.h"
#include "ui_edit_profile.h"
#include "core/profile.h"

#include <QSettings>

namespace forms
{

EditProfile::EditProfile(Profile & pProfile, QWidget * parent) : QDialog(parent, Qt::Dialog), mProfile(pProfile)
{
	ui = new Ui::EditProfile();
	ui->setupUi(this);

	ui->txtName->setText(mProfile.mName);
	ui->txtAccount->setText(mProfile.mAccount);
}

EditProfile::~EditProfile()
{
	delete ui;
}

void EditProfile::on_btnBox_accepted()
{
	mProfile.mName = ui->txtName->text();
	mProfile.mAccount = ui->txtAccount->text();
	mProfile.save();
	accept();
}

} // namespace forms
