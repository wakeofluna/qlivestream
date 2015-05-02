#include "config.h"
#include "edit_profile.h"
#include "ui_edit_profile.h"
#include "core/profile.h"

#include <QCheckBox>
#include <QSettings>

namespace forms
{

EditProfile::EditProfile(Profile & pProfile, QWidget * parent) : QDialog(parent, Qt::Dialog), mProfile(pProfile)
{
	ui = new Ui::EditProfile();
	ui->setupUi(this);

	ui->txtName->setText(mProfile.mName);
	ui->txtAccount->setText(mProfile.mAccount);

	mCheckboxes.reserve(AuthScope::max);
	for (int i = 0; i < AuthScope::max; ++i)
	{
		AuthScope lScope((AuthScope::Scope)i);

		QCheckBox * lCheckBox = new QCheckBox(this);
		lCheckBox->setText(lScope.description());
		if (mProfile.mRequested.test(lScope))
			lCheckBox->setCheckState(Qt::Checked);

		mCheckboxes.append(lCheckBox);
		ui->layScopes->addWidget(lCheckBox);
	}
}

EditProfile::~EditProfile()
{
	delete ui;
}

void EditProfile::on_btnBox_accepted()
{
	mProfile.mName = ui->txtName->text();
	mProfile.mAccount = ui->txtAccount->text();

	for (int i = 0; i < AuthScope::max; ++i)
	{
		AuthScope lScope((AuthScope::Scope)i);
		if (mCheckboxes[i]->checkState() == Qt::Checked)
			mProfile.mRequested.set(lScope);
		else
			mProfile.mRequested.reset(lScope);
	}

	mProfile.save();
	accept();
}

} // namespace forms
