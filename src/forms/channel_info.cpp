#include "config.h"
#include "channel_info.h"
#include "ui_channel_info.h"
#include "core/channel_chat.h"
#include "core/channel_object.h"

#include <QDesktopServices>

namespace forms
{

ChannelInfo::ChannelInfo(ChannelObject & pChannel, QWidget * parent) : QWidget(parent), mChannel(pChannel)
{
	ui = new Ui::ChannelInfo();
	ui->setupUi(this);

	setWindowTitle(mChannel.name());
	ui->txtChannel->setText(mChannel.name());

	ChannelChat * lChat = mChannel.chat();
	if (lChat == nullptr)
	{
		ui->btnOpenChat->setEnabled(false);
		ui->btnOpenChat->setToolTip(tr("Chat option not supported"));
	}
	else
	{
		connect(lChat, &ChannelChat::chatConnected, this, &ChannelInfo::chatConnected);
		connect(lChat, &ChannelChat::chatError, this, &ChannelInfo::chatError);
		connect(lChat, &ChannelChat::chatDisconnected, this, &ChannelInfo::chatDisconnected);

		if (lChat->isConnected())
			chatConnected();
	}

	connect(&mChannel, &ChannelObject::statsChanged, this, &ChannelInfo::updateFromChannel);
	updateFromChannel();
}

ChannelInfo::~ChannelInfo()
{
	delete ui;
}

void ChannelInfo::requestUpdateChannel()
{
	mChannel.requestUpdate();
}

void ChannelInfo::chatConnected()
{
	ui->txtChat->setEnabled(true);
	ui->txtMessage->setEnabled(true);
	ui->btnChat->setEnabled(true);
}

void ChannelInfo::chatError(QString pMessage)
{
}

void ChannelInfo::on_btnOpenChat_clicked()
{
	ChannelChat * lChat = mChannel.chat();
	Q_ASSERT(lChat != nullptr);
	lChat->connectToChat();
}

void ChannelInfo::chatDisconnected()
{
	ui->btnChat->setEnabled(false);
}

void ChannelInfo::on_chkPartner_clicked()
{
	ui->chkPartner->setChecked(mChannel.isPartnered());
}

void ChannelInfo::on_btnUpdate_clicked()
{
	mChannel.updateStreamSettings(ui->txtTitle->text(), ui->txtPlaying->text(), ui->chkMature->isChecked(), ui->sliDelay->value());
}

void ChannelInfo::on_btnOpenStream_clicked()
{
	// TODO check preferences
	QUrl lUrl;

	lUrl = mChannel.getStreamUrl(ChannelObject::URL_STREAM_WEBSITE);
	if (lUrl.isValid())
		QDesktopServices::openUrl(lUrl);
}

void ChannelInfo::on_chkMature_clicked()
{
	if (!mChannel.isEditor())
		ui->chkMature->setChecked(mChannel.isMature());
	else
		ui->btnUpdate->setEnabled(true);
}

void ChannelInfo::on_txtTitle_textEdited(QString pText)
{
	ui->btnUpdate->setEnabled(true);
}

void ChannelInfo::on_txtPlaying_textEdited(QString pText)
{
	ui->btnUpdate->setEnabled(true);
}

void ChannelInfo::on_sliDelay_actionTriggered(int pAction)
{
	ui->btnUpdate->setEnabled(true);
}

void ChannelInfo::on_sliDelay_valueChanged(int pValue)
{
	ui->lblNumDelay->setText(QString::number(pValue));
}

void ChannelInfo::updateFromChannel()
{
	ui->txtChannel->setText(mChannel.displayName());
	ui->chkPartner->setChecked(mChannel.isPartnered());
	ui->lblNumFollowers->setText(QString::number(mChannel.numFollowers()));
	ui->lblNumViews->setText(QString::number(mChannel.numViews()));
	ui->txtTitle->setText(mChannel.status());
	ui->txtPlaying->setText(mChannel.category());
	ui->chkMature->setChecked(mChannel.isMature());

	int lDelay = mChannel.delay();
	ui->lblDelay->setVisible(lDelay >= 0);
	ui->sliDelay->setVisible(lDelay >= 0);
	ui->lblNumDelay->setVisible(lDelay >= 0);
	ui->lblDelaySeconds->setVisible(lDelay >= 0);
	ui->sliDelay->setValue(lDelay);

	bool lIsEditor = mChannel.isEditor();
	ui->btnUpdate->setVisible(lIsEditor);
	ui->txtTitle->setReadOnly(!lIsEditor);
	ui->txtPlaying->setReadOnly(!lIsEditor);
	ui->sliDelay->setEnabled(lIsEditor);

	ui->btnUpdate->setEnabled(false);
}

} // namespace forms
