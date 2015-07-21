#include "config.h"
#include "channel_info.h"
#include "ui_channel_info.h"
#include "core/channel_chat.h"
#include "core/channel_object.h"
#include "core/channel_chatter.h"
#include "core/profile.h"

#include <QDesktopServices>

namespace forms
{

ChannelInfo::ChannelInfo(ChannelObject & pChannel, QWidget * parent) : QWidget(parent), mChannel(pChannel)
{
	ui = new Ui::ChannelInfo();
	ui->setupUi(this);

	setWindowTitle(mChannel.name());
	ui->txtChannel->setText(mChannel.name());

	if (mChannel.profile().level() == Profile::ANONYMOUS)
	{
		ui->txtMessage->setVisible(false);
		ui->btnChat->setVisible(false);
	}

	ChannelChat * lChat = mChannel.chat();
	if (lChat == nullptr)
	{
		ui->btnOpenChat->setEnabled(false);
		ui->btnOpenChat->setToolTip(tr("Chat option not supported"));
		ui->btnCloseChat->setVisible(false);
	}
	else
	{
		connect(lChat, &ChannelChat::chatStateChanged, this, &ChannelInfo::chatStateChanged);
		connect(lChat, &ChannelChat::chatError, this, &ChannelInfo::chatError);
		connect(lChat, &ChannelChat::chatterNew, this, &ChannelInfo::chatterNew);
		connect(lChat, &ChannelChat::chatterChanged, this, &ChannelInfo::chatterChanged);
		connect(lChat, &ChannelChat::chatterLost, this, &ChannelInfo::chatterLost);
		connect(lChat, &ChannelChat::chatMessage, this, &ChannelInfo::chatMessage);
		chatStateChanged();
		ui->txtChat->clear();
	}

	connect(&mChannel, &ChannelObject::statsChanged, this, &ChannelInfo::updateFromChannel);
	updateFromChannel();
}

ChannelInfo::~ChannelInfo()
{
	ChannelChat * lChat = mChannel.chat();
	if (lChat != nullptr)
		lChat->disconnectFromChat();

	delete ui;
}

void ChannelInfo::requestUpdateChannel()
{
	mChannel.requestUpdate();
}

void ChannelInfo::chatStateChanged()
{
	ChannelChat * lChat = mChannel.chat();
	Q_ASSERT(lChat != nullptr);

	switch (lChat->state())
	{
		case ChannelChat::NONE:
			ui->btnOpenChat->setVisible(true);
			ui->btnCloseChat->setVisible(false);
			ui->txtChat->append("Disconnected!");
			break;

		case ChannelChat::JOINING:
			ui->btnOpenChat->setVisible(false);
			ui->btnCloseChat->setVisible(true);
			ui->txtChat->append("Connecting to chat...");
			break;

		case ChannelChat::JOINED:
			ui->txtChat->setEnabled(true);
			ui->txtMessage->setEnabled(true);
			ui->lstChatUsers->setEnabled(true);
			ui->btnChat->setEnabled(true);
			ui->btnCloseChat->setEnabled(true);
			ui->txtChat->append("Connected!");
			break;

		case ChannelChat::LEAVING:
			ui->btnCloseChat->setEnabled(false);
			ui->btnChat->setEnabled(false);
			ui->lstChatUsers->setEnabled(false);
			ui->txtChat->append("Disconnecting...");
			break;
	}
}

void ChannelInfo::chatError(QString pMessage)
{
	ui->txtChat->append(pMessage);
}

void ChannelInfo::chatterNew(ChannelChatter & pChatter)
{
	//ui->txtChat->append(QString("(%1 joined)").arg(pChatter.displayName()));
}

void ChannelInfo::chatterChanged(ChannelChatter & pChatter)
{

}

void ChannelInfo::chatterLost(ChannelChatter & pChatter)
{
	//ui->txtChat->append(QString("(%1 left)").arg(pChatter.displayName()));
}

void ChannelInfo::chatMessage(ChannelChatter & pChatter, QString pMessage, ChannelChat::SmileyList const & pSmilies)
{
	ChannelChatter::Color lColor = pChatter.color();
	QString lMessage = pMessage.replace('<', "&lt;").replace('>', "&gt;");

	QString lText = QString("<font color='%1'><b>%2</b></font>: %3").arg(QColor(lColor.r, lColor.g, lColor.b).name()).arg(pChatter.displayName()).arg(lMessage);
	ui->txtChat->append(lText);
}

void ChannelInfo::on_btnOpenChat_clicked()
{
	ChannelChat * lChat = mChannel.chat();
	Q_ASSERT(lChat != nullptr);
	lChat->connectToChat();
}

void ChannelInfo::on_btnCloseChat_clicked()
{
	ChannelChat * lChat = mChannel.chat();
	Q_ASSERT(lChat != nullptr);
	lChat->disconnectFromChat();
}

void ChannelInfo::on_btnChat_clicked()
{
	ChannelChat * lChat = mChannel.chat();
	Q_ASSERT(lChat != nullptr);

	QString lText = ui->txtMessage->text();
	ui->txtMessage->clear();

	lChat->sendMessage(lText);
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
