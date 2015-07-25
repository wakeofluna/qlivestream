#include "config.h"
#include "channel_info.h"
#include "ui_channel_info.h"
#include "core/channel_chat.h"
#include "core/channel_object.h"
#include "core/channel_chatter.h"
#include "core/profile.h"

#include <QDebug>
#include <QDesktopServices>
#include <QFile>
#include <QProcess>
#include <QTime>
#include <QTimer>

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

	if (!mChannel.isPartnered() || !mChannel.isEditor())
		ui->frmSubscribers->hide();

	mViewerProcess = nullptr;

	mUpdateCounter = -1;
	mUpdateTimer = new QTimer(this);
	mUpdateTimer->setInterval(10000);
	mUpdateTimer->start();
	connect(mUpdateTimer, &QTimer::timeout, this, &ChannelInfo::onUpdateTimer);
	onUpdateTimer();
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
			ui->lstChatUsers->clear();
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
			//ui->txtChat->append("Disconnecting...");
			break;
	}
}

void ChannelInfo::chatError(QString pMessage)
{
	ui->txtChat->append(pMessage);
}

void ChannelInfo::chatterNew(ChannelChatter & pChatter)
{
	//addChatMessage(QString("(%1 joined)").arg(pChatter.displayName()));
	ui->lstChatUsers->addItem(pChatter.displayName());
	ui->lstChatUsers->sortItems();
}

void ChannelInfo::chatterChanged(ChannelChatter & pChatter)
{
	for (int i = 0; i < ui->lstChatUsers->count(); ++i)
	{
		QListWidgetItem * lItem = ui->lstChatUsers->item(i);
		QString lName = lItem->text();
		if (lName.startsWith('#') || lName.startsWith('@') || lName.startsWith('*'))
			lName = lName.mid(1);

		if (lName.compare(pChatter.displayName(), Qt::CaseInsensitive) == 0)
		{
			QString lNew;

			if (pChatter.isOwner())
				lNew.append('#');
			else if (pChatter.isModerator())
				lNew.append('@');
			else if (pChatter.isSelf())
				lNew.append('*');

			lNew.append(pChatter.displayName());
			lItem->setText(lNew);
			break;
		}
	}

	ui->lstChatUsers->sortItems();
}

void ChannelInfo::chatterLost(ChannelChatter & pChatter)
{
	//addChatMessage(QString("(%1 left)").arg(pChatter.displayName()));

	QList<QListWidgetItem *> lRemove;
	lRemove.reserve(1);

	for (int i = 0; i < ui->lstChatUsers->count(); ++i)
	{
		QListWidgetItem * lItem = ui->lstChatUsers->item(i);
		QString lName = lItem->text();
		if (lName.startsWith('#') || lName.startsWith('@') || lName.startsWith('*'))
			lName = lName.mid(1);

		if (lName.compare(pChatter.displayName(), Qt::CaseInsensitive) == 0)
			lRemove.append(lItem);
	}

	for (QListWidgetItem * lItem : lRemove)
		delete lItem;
}

void ChannelInfo::chatMessage(ChannelChatter & pChatter, QString pMessage, ChannelChat::SmileyList const & pSmilies)
{
	ChannelChatter::Color lColor = pChatter.color();
	QString lMessage = pMessage.replace('<', "&lt;").replace('>', "&gt;");

	QString lText = QString("<font color='%1'><b>%2</b></font>: %3")
			.arg(QColor(lColor.r, lColor.g, lColor.b).name())
			.arg(pChatter.displayName())
			.arg(lMessage);

	addChatMessage(lText);
}

void ChannelInfo::onUpdateTimer()
{
	if (++mUpdateCounter == 3)
		mUpdateCounter = 0;

	switch (mUpdateCounter)
	{
		case 0:
			mChannel.profile().getChannelStream(mChannel);
			break;

		case 1:
			if ((mChannel.isOnline() && mChannel.isEditor()) || mChannel.numFollowers() == -1)
			{
				mChannel.profile().getChannelFollowers(mChannel);
			}
			break;

		case 2:
			if ((mChannel.isOnline() && mChannel.isEditor()) || mChannel.numSubscribers() == -1)
			{
				mChannel.profile().getChannelSubscribers(mChannel);
			}
			break;
	}
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

	QString lProgram;

	lProgram = "/usr/bin/livestreamer";
	if (QFile::exists(lProgram))
	{
		if (mViewerProcess != nullptr && mViewerProcess->state() != QProcess::NotRunning)
		{
			qDebug() << "Viewer process still running!";
			return;
		}

		delete mViewerProcess;
		mViewerProcess = new QProcess(this);

		QStringList lArguments;
		lArguments << "--no-version-check";
		lArguments << "--player" << "/usr/bin/mpv --untimed";
		lArguments << "--player-passthrough" << "rtmp,hls";
		lArguments << QString("http://www.twitch.tv/%1").arg(mChannel.name());
		lArguments << "source,best";

		mViewerProcess->setProcessChannelMode(QProcess::ForwardedChannels);
		mViewerProcess->setProgram(lProgram);
		mViewerProcess->setArguments(lArguments);
		mViewerProcess->start();
		return;
	}

	// Fallback to browser
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
	ui->lblNumViewers->setText(QString::number(mChannel.numViewers()));
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

void ChannelInfo::addChatMessage(QString pMessage)
{
	QTime lNow = QTime::currentTime();

	QString lText = QString("<font color='#666666'>[%1]</font> %2")
			.arg(lNow.toString(Qt::SystemLocaleShortDate))
			.arg(pMessage);

	ui->txtChat->append(lText);
}

} // namespace forms
