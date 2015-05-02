#include "config.h"
#include "edit_profile.h"
#include "ui_edit_profile.h"
#include "core/profile.h"

#include <QCheckBox>
#include <QPushButton>
#include <QSettings>

namespace forms
{

EditProfile::EditProfile(Profile & pProfile, QWidget * parent) : QDialog(parent, Qt::Dialog), mProfile(pProfile)
{
	ui = new Ui::EditProfile();
	ui->setupUi(this);

	ui->txtName->setText(mProfile.mName);
	ui->txtAccount->setText(mProfile.mAccount);

	bool lIsNewProfile = mProfile.mAccount.isEmpty();

	mCheckboxes.reserve(AuthScope::max);
	for (int i = 0; i < AuthScope::max; ++i)
	{
		AuthScope lScope((AuthScope::Scope)i);
		if (lIsNewProfile && lScope.isDefault())
			mProfile.mRequested.set(lScope);

		QCheckBox * lCheckBox = new QCheckBox(this);
		lCheckBox->setText(lScope.description());
		if (mProfile.mRequested.test(lScope))
			lCheckBox->setCheckState(Qt::Checked);

		mCheckboxes.append(lCheckBox);
		ui->layScopes->addWidget(lCheckBox);
	}

	enableSave();
}

EditProfile::~EditProfile()
{
	delete ui;
}

void EditProfile::on_txtName_textChanged(QString const & pText)
{
	enableSave();
}

void EditProfile::on_txtAccount_textChanged(QString const & pText)
{
	enableSave();
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

void EditProfile::enableSave() const
{
	QPushButton * lButton = ui->btnBox->button(QDialogButtonBox::Save);
	lButton->setEnabled(!ui->txtName->text().isEmpty() && !ui->txtAccount->text().isEmpty());
}

} // namespace forms
