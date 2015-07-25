#ifndef FORMS_CHANNEL_INFO_H_
#define FORMS_CHANNEL_INFO_H_

#include <QWidget>
#include <QString>
#include "core/channel_chat.h"
class ChannelChatter;
class ChannelObject;
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
	explicit ChannelInfo(ChannelObject & pChannel, QWidget * parent);
	~ChannelInfo();

public slots:
	void requestUpdateChannel();

protected slots:
	void on_chkPartner_clicked();
	void on_chkMature_clicked();
	void on_txtTitle_textEdited(QString pText);
	void on_txtPlaying_textEdited(QString pText);
	void on_sliDelay_actionTriggered(int pAction);
	void on_sliDelay_valueChanged(int pValue);
	void on_btnUpdate_clicked();
	void on_btnOpenStream_clicked();
	void on_btnOpenChat_clicked();
	void on_btnCloseChat_clicked();
	void on_btnChat_clicked();

private slots:
	void chatStateChanged();
	void chatError(QString pMessage);
	void chatterNew(ChannelChatter & pChatter);
	void chatterChanged(ChannelChatter & pChatter);
	void chatterLost(ChannelChatter & pChatter);
	void chatMessage(ChannelChatter & pChatter, QString pMessage, ChannelChat::SmileyList const & pSmilies);
	void onUpdateTimer();

private:
	void updateFromChannel();
	void addChatMessage(QString pMessage);

	Ui::ChannelInfo * ui;
	ChannelObject & mChannel;
	QProcess * mViewerProcess;
	QTimer * mUpdateTimer;
	int mUpdateCounter;
};

} // namespace forms

#endif // FORMS_CHANNEL_INFO_H_
