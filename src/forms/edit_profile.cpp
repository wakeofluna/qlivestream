#include "config.h"
#include "edit_profile.h"
#include "ui_edit_profile.h"
#include "core/config_profile.h"

#include <QCheckBox>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>

namespace forms
{

EditProfile::EditProfile(ConfigProfile & pProfile, QWidget * parent) : QDialog(parent, Qt::Dialog), mProfile(pProfile)
{
	ui = new Ui::EditProfile();
	ui->setupUi(this);

	// TODO iterate this from somewhere
	ui->cbbService->addItem("Twitch.TV");

	ui->txtAccount->setText(mProfile.account());
	ui->cbbService->setCurrentText(mProfile.service());
	ui->optClient->setChecked(mProfile.level() == Profile::CLIENT);
	ui->optModerator->setChecked(mProfile.level() == Profile::MODERATOR);
	ui->optStreamer->setChecked(mProfile.level() == Profile::STREAMER);

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
		mProfile.setLevel(Profile::CLIENT);
		if (ui->optModerator->isChecked()) mProfile.setLevel(Profile::MODERATOR);
		if (ui->optStreamer->isChecked()) mProfile.setLevel(Profile::STREAMER);
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
