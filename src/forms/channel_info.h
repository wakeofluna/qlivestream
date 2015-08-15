#ifndef FORMS_CHANNEL_INFO_H_
#define FORMS_CHANNEL_INFO_H_

#include <QWidget>
#include <QString>

#include "../core/i_channel_chat.h"
class IChannelUser;
class IChannel;
class QProcess;
class QTimer;

namespace Ui
{
	class ChannelInfo;
}

namespace forms
{

class ChannelInfo : public QWidget
{
Q_OBJECT

public:
	explicit ChannelInfo(IChannel & pChannel, QWidget * parent);
	~ChannelInfo();

public slots:
	void requestUpdateChannel();

signals:
	void showVideos(IChannel * pChannel = nullptr);

protected slots:
	void on_chkPartner_clicked();
	void on_chkMature_clicked();
	void on_txtTitle_textEdited(QString pText);
	void on_txtPlaying_textEdited(QString pText);
	void on_sliDelay_actionTriggered(int pAction);
	void on_sliDelay_valueChanged(int pValue);
	void on_btnFollow_clicked();
	void on_btnUnfollow_clicked();
	void on_btnVideos_clicked();
	void on_btnUpdate_clicked();
	void on_btnOpenStream_clicked();
	void on_btnOpenChat_clicked();
	void on_btnCloseChat_clicked();
	void on_btnChat_clicked();

private slots:
	void chatStateChanged();
	void chatError(QString pMessage);
	void chatterNew(IChannelUser & pChatter);
	void chatterChanged(IChannelUser & pChatter);
	void chatterLost(IChannelUser & pChatter);
	void chatMessage(IChannelUser & pChatter, QString pMessage, IChannelChat::SmileyList const & pSmilies);
	void onUpdateTimer();

private:
	void updateFromChannel();
	void addChatMessage(QString pMessage);

	Ui::ChannelInfo * ui;
	IChannel & mChannel;
	QProcess * mViewerProcess;
	QTimer * mUpdateTimer;
	int mUpdateCounter;
};

} // namespace forms

#endif // FORMS_CHANNEL_INFO_H_
