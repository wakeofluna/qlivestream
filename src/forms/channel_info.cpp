#include "config.h"
#include "channel_info.h"
#include "ui_channel_info.h"
#include <QDebug>
#include <QDesktopServices>
#include <QFile>
#include <QProcess>
#include <QTime>
#include <QTimer>

#include "../core/i_channel_user.h"
#include "../core/i_channel.h"
#include "../core/i_channel_chat.h"
#include "../core/i_profile.h"
#include "../core/i_user.h"

namespace forms
{

ChannelInfo::ChannelInfo(IChannel & pChannel, QWidget * parent) : QWidget(parent), mChannel(pChannel)
{
	ui = new Ui::ChannelInfo();
	ui->setupUi(this);

	setWindowTitle(mChannel.owner().name());
	ui->txtChannel->setText(mChannel.owner().name());

	if (mChannel.owner().profile().level() == IProfile::ANONYMOUS)
	{
		ui->txtMessage->setVisible(false);
		ui->btnChat->setVisible(false);
	}

	if (mChannel.owner().profile().level() == IProfile::ANONYMOUS || mChannel.owner().isSelf())
	{
		ui->btnFollow->setEnabled(false);
		ui->btnUnfollow->setEnabled(false);
	}

	IChannelChat * lChat = mChannel.chat();
	if (lChat == nullptr)
	{
		ui->btnOpenChat->setEnabled(false);
		ui->btnOpenChat->setToolTip(tr("Chat option not supported"));
		ui->btnCloseChat->setVisible(false);
	}
	else
	{
		connect(lChat, &IChannelChat::stateChanged, this, &ChannelInfo::chatStateChanged);
		connect(lChat, &IChannelChat::chatError, this, &ChannelInfo::chatError);
		connect(lChat, &IChannelChat::chatterNew, this, &ChannelInfo::chatterNew);
		connect(lChat, &IChannelChat::chatterChanged, this, &ChannelInfo::chatterChanged);
		connect(lChat, &IChannelChat::chatterLost, this, &ChannelInfo::chatterLost);
		connect(lChat, &IChannelChat::chatMessage, this, &ChannelInfo::chatMessage);
		chatStateChanged();
		ui->txtChat->clear();
	}

	connect(&mChannel, &IChannel::infoUpdated, this, &ChannelInfo::updateFromChannel);
	updateFromChannel();

	if (!mChannel.isPartnered() || !mChannel.amEditor())
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
	IChannelChat * lChat = mChannel.chat();
	if (lChat != nullptr)
		lChat->disconnectFromChat();

	delete ui;
}

void ChannelInfo::requestUpdateChannel()
{
	mChannel.refresh();
}

void ChannelInfo::chatStateChanged()
{
	IChannelChat * lChat = mChannel.chat();
	Q_ASSERT(lChat != nullptr);

	switch (lChat->state())
	{
		case IChannelChat::NONE:
			ui->lstChatUsers->clear();
			ui->btnOpenChat->setVisible(true);
			ui->btnCloseChat->setVisible(false);
			ui->txtChat->append("Disconnected!");
			break;

		case IChannelChat::JOINING:
			ui->btnOpenChat->setVisible(false);
			ui->btnCloseChat->setVisible(true);
			ui->txtChat->append("Connecting to chat...");
			break;

		case IChannelChat::JOINED:
			ui->txtChat->setEnabled(true);
			ui->txtMessage->setEnabled(true);
			ui->lstChatUsers->setEnabled(true);
			ui->btnChat->setEnabled(true);
			ui->btnCloseChat->setEnabled(true);
			ui->txtChat->append("Connected!");
			break;

		case IChannelChat::LEAVING:
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

void ChannelInfo::chatterNew(IChannelUser & pChatter)
{
	//addChatMessage(QString("(%1 joined)").arg(pChatter.displayName()));
	ui->lstChatUsers->addItem(pChatter.displayName());
	ui->lstChatUsers->sortItems();
}

void ChannelInfo::chatterChanged(IChannelUser & pChatter)
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

void ChannelInfo::chatterLost(IChannelUser & pChatter)
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

void ChannelInfo::chatMessage(IChannelUser & pChatter, QString pMessage, IChannelChat::SmileyList const & pSmilies)
{
	IChannelUser::Color lColor = pChatter.color();
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
			mChannel.refresh();
			break;

		case 1:
			if ((mChannel.isOnline() && mChannel.amEditor()) || mChannel.numFollowers() == -1)
			{
				//mChannel.profile().getChannelFollowers(mChannel);
			}
			break;

		case 2:
			if ((mChannel.isOnline() && mChannel.amEditor()) || mChannel.numSubscribers() == -1)
			{
				//mChannel.profile().getChannelSubscribers(mChannel);
			}
			break;
	}
}

void ChannelInfo::on_btnOpenChat_clicked()
{
	IChannelChat * lChat = mChannel.chat();
	Q_ASSERT(lChat != nullptr);
	lChat->connectToChat();
}

void ChannelInfo::on_btnCloseChat_clicked()
{
	IChannelChat * lChat = mChannel.chat();
	Q_ASSERT(lChat != nullptr);
	lChat->disconnectFromChat();
}

void ChannelInfo::on_btnChat_clicked()
{
	IChannelChat * lChat = mChannel.chat();
	Q_ASSERT(lChat != nullptr);

	QString lText = ui->txtMessage->text();
	ui->txtMessage->clear();

	lChat->sendMessage(lText);
}

void ChannelInfo::on_chkPartner_clicked()
{
	ui->chkPartner->setChecked(mChannel.isPartnered());
}

void ChannelInfo::on_btnFollow_clicked()
{
	mChannel.follow();
}

void ChannelInfo::on_btnUnfollow_clicked()
{
	mChannel.unfollow();
}

void ChannelInfo::on_btnVideos_clicked()
{
	emit showVideos(&mChannel);
}

void ChannelInfo::on_btnUpdate_clicked()
{
	ICategory * lCategory = mChannel.owner().profile().getCategoryFor(ui->txtPlaying->text(), false);
	mChannel.modifyStreamSettings(ui->txtTitle->text(), lCategory, ui->chkMature->isChecked(), ui->sliDelay->value());
}

void ChannelInfo::on_btnOpenStream_clicked()
{
	// TODO check preferences

	QUrl lUrl;
	QString lProgram;
	QStringList lArguments;

	lUrl = mChannel.streamUrl(IChannel::URL_STREAM_DIRECT);
	if (lUrl.isValid())
	{
		lProgram = "/usr/bin/mpv";
		lArguments << "--untimed";
		lArguments << "--quiet";
		lArguments << lUrl.toString(QUrl::FullyEncoded);
	}
	else
	{
		lProgram = "/usr/bin/livestreamer";
		lArguments << "--no-version-check";
		lArguments << "--player" << "/usr/bin/mpv --untimed";
		lArguments << "--player-passthrough" << "rtmp,hls";
		lArguments << QString("http://www.twitch.tv/%1").arg(mChannel.owner().name());
		lArguments << "source,best";
	}

	if (!lProgram.isEmpty() && QFile::exists(lProgram))
	{
		if (mViewerProcess != nullptr && mViewerProcess->state() != QProcess::NotRunning)
		{
			qDebug() << "Viewer process still running!";
			return;
		}

		delete mViewerProcess;
		mViewerProcess = new QProcess(this);

		mViewerProcess->setProcessChannelMode(QProcess::ForwardedChannels);
		mViewerProcess->setProgram(lProgram);
		mViewerProcess->setArguments(lArguments);
		mViewerProcess->start();
		return;
	}

	// Fallback to browser
	lUrl = mChannel.streamUrl(IChannel::URL_STREAM_WEBSITE);
	if (lUrl.isValid())
		QDesktopServices::openUrl(lUrl);
}

void ChannelInfo::on_chkMature_clicked()
{
	if (!mChannel.amEditor())
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
	ui->btnFollow->setVisible(!mChannel.isFollowed());
	ui->btnUnfollow->setVisible(mChannel.isFollowed());
	ui->lblNumFollowers->setText(QString::number(mChannel.numFollowers()));
	ui->lblNumViews->setText(QString::number(mChannel.numViews()));
	ui->lblNumViewers->setText(QString::number(mChannel.numViewers()));
	ui->txtTitle->setText(mChannel.title());
	ui->txtPlaying->setText(mChannel.category() ? mChannel.category()->name() : QString());
	ui->chkMature->setChecked(mChannel.isMature());

	int lDelay = mChannel.delay();
	ui->lblDelay->setVisible(lDelay >= 0);
	ui->sliDelay->setVisible(lDelay >= 0);
	ui->lblNumDelay->setVisible(lDelay >= 0);
	ui->lblDelaySeconds->setVisible(lDelay >= 0);
	ui->sliDelay->setValue(lDelay);

	bool lIsEditor = mChannel.amEditor();
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
