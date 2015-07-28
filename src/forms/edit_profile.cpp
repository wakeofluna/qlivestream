#include "config.h"
#include "edit_profile.h"
#include "ui_edit_profile.h"
#include "core/config_profile.h"
#include "core/i_profile.h"

#include <QMessageBox>
#include <QPushButton>

namespace forms
{

EditProfile::EditProfile(ConfigProfile & pProfile, QWidget * parent) : QDialog(parent, Qt::Dialog), mProfile(pProfile)
{
	ui = new Ui::EditProfile();
	ui->setupUi(this);

	QStringList lFactories = listServices();
	for (QString & i : lFactories)
		ui->cbbService->addItem(i);

	ui->txtAccount->setText(mProfile.account());
	ui->cbbService->setCurrentText(mProfile.service());
	ui->optAnonymous->setChecked(mProfile.level() == IProfile::ANONYMOUS);
	ui->optViewer->setChecked(mProfile.level() == IProfile::VIEWER);
	ui->optModerator->setChecked(mProfile.level() == IProfile::EDITOR);
	ui->optStreamer->setChecked(mProfile.level() == IProfile::STREAMER);

	bool lIsNewProfile = mProfile.account().isEmpty();
	if (lIsNewProfile)
		ui->btnBox->button(QDialogButtonBox::Discard)->setEnabled(false);

	enableSave();
}

EditProfile::~EditProfile()
{
	delete ui;
}

void EditProfile::on_txtAccount_textChanged(QString const & pText)
{
	enableSave();
}

void EditProfile::on_btnBox_clicked(QAbstractButton * pButton)
{
	if (pButton == ui->btnBox->button(QDialogButtonBox::Save))
	{
		mProfile.setAccount(ui->txtAccount->text());
		mProfile.setService(ui->cbbService->currentText());
		mProfile.setLevel(IProfile::ANONYMOUS);
		if (ui->optViewer->isChecked()) mProfile.setLevel(IProfile::VIEWER);
		if (ui->optModerator->isChecked()) mProfile.setLevel(IProfile::EDITOR);
		if (ui->optStreamer->isChecked()) mProfile.setLevel(IProfile::STREAMER);
		mProfile.save();
		accept();
	}
	else if (pButton == ui->btnBox->button(QDialogButtonBox::Discard))
	{
		int lAnswer = QMessageBox::question(this, tr("Remove profile"), tr("Are you sure you want to remove this profile?"));
		if (lAnswer == QDialogButtonBox::Yes)
		{
			mProfile.erase();
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
	lButton->setEnabled(!ui->txtAccount->text().isEmpty());
}

} // namespace forms
