#ifndef FORMS_DEBUG_NETWORK_MESSAGES_H_
#define FORMS_DEBUG_NETWORK_MESSAGES_H_

#include <QWidget>
class QTreeWidgetItem;
class QVariant;

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
	explicit DebugNetworkMessages(QWidget *parent = nullptr);
	~DebugNetworkMessages();

	bool isCapturing() const;
	void setCapturing(bool pCapture);

	void addMessage(QString pTag, QVariant const & pMessage);
	void addError(QString pTag, QString const & pError, QVariant const & pMessage);

public slots:
	void clear();

signals:
	void capturingChanged();
	void messageAdded();
	void cleared();

private:
	void addEntry(QString pTag, QString const & pStatus, QVariant const & pMessage);
	Ui::DebugNetworkMessages * ui;
};

} // namespace forms

#endif // FORMS_DEBUG_NETWORK_MESSAGES_H_
