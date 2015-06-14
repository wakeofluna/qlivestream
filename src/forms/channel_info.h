#ifndef FORMS_CHANNEL_INFO_H_
#define FORMS_CHANNEL_INFO_H_

#include <QWidget>
#include <QString>
class ChannelObject;

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

private slots:
	void chatConnected();
	void chatError(QString pMessage);
	void chatDisconnected();

private:
	void updateFromChannel();

	Ui::ChannelInfo * ui;
	ChannelObject & mChannel;
};

} // namespace forms

#endif // FORMS_CHANNEL_INFO_H_
