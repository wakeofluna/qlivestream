#ifndef FORMS_DEBUG_NETWORK_MESSAGES_H_
#define FORMS_DEBUG_NETWORK_MESSAGES_H_

#include <QWidget>
class QTreeWidgetItem;

namespace Ui
{
	class DebugNetworkMessages;
}

namespace forms
{

class DebugNetworkMessages : public QWidget
{
Q_OBJECT

public:
	DebugNetworkMessages(QWidget *parent = 0);
	~DebugNetworkMessages();

	bool isCapturing() const;
	void setCapturing(bool pCapture);

	void addMessage(QString pTag, QVariant const & pMessage);
	void addError(QString pTag, QString const & pMessage);

public slots:
	void clear();

signals:
	void capturingChanged();
	void messageAdded();
	void cleared();

private:
	Ui::DebugNetworkMessages * ui;
};

} // namespace forms

#endif // FORMS_DEBUG_NETWORK_MESSAGES_H_
