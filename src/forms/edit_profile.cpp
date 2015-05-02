#include "config.h"
#include "edit_profile.h"
#include "ui_edit_profile.h"
#include "core/profile.h"

#include <QCheckBox>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>

namespace forms
{

EditProfile::EditProfile(Profile & pProfile, QWidget * parent) : QDialog(parent, Qt::Dialog), mProfile(pProfile)
{
	ui = new Ui::EditProfile();
	ui->setupUi(this);

	mOriginalName = mProfile.mName;
	ui->txtName->setText(mProfile.mName);
	ui->txtAccount->setText(mProfile.mAccount);

	bool lIsNewProfile = mProfile.mAccount.isEmpty();
	if (lIsNewProfile)
		ui->btnBox->button(QDialogButtonBox::Discard)->setEnabled(false);

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

void EditProfile::on_btnBox_clicked(QAbstractButton * pButton)
{
	if (pButton == ui->btnBox->button(QDialogButtonBox::Save))
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

		mProfile.saveAndReplace(mOriginalName);
		accept();
	}
	else if (pButton == ui->btnBox->button(QDialogButtonBox::Discard))
	{
		int lAnswer = QMessageBox::question(this, tr("Remove profile"), tr("Are you sure you want to remove this profile?"));
		if (lAnswer == QDialogButtonBox::Yes)
		{
			Profile::erase(mOriginalName);
			mProfile.mName.clear();
			mProfile.mAccount.clear();
			accept();
		}
	}
	else if (pButton == ui->btnBox->button(QDialogButtonBox::Cancel))
	{
		reject();
	}
}

void EditProfile::enableSave() const
{
	QPushButton * lButton = ui->btnBox->button(QDialogButtonBox::Save);
	lButton->setEnabled(!ui->txtName->text().isEmpty() && !ui->txtAccount->text().isEmpty());
}

} // namespace forms
