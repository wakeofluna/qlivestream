#ifndef FORMS_CHANNEL_OBJECT_WIDGET_H_
#define FORMS_CHANNEL_OBJECT_WIDGET_H_

#include "core/storage_access.h"
#include <QWidget>
class QByteArray;
class IChannel;

namespace Ui
{
	class ChannelObjectWidget;
}

namespace forms
{

class ChannelObjectWidget : public QWidget, public StorageAccess
{
Q_OBJECT

public:
	ChannelObjectWidget(IChannel & pObject, QWidget * parent);
	~ChannelObjectWidget();

	inline IChannel & object() const { return mChannel; }
	inline IChannel & channel() const { return mChannel; }

	void setLogo(QByteArray const & pData);
	bool operator< (ChannelObjectWidget const & pOther) const;

public slots:
	void updateFromObject();

signals:
	void clicked(IChannel * pCategory = nullptr);

private slots:
	void on_btnChannel_clicked();

private:
	Ui::ChannelObjectWidget * ui;
	IChannel & mChannel;
};

} // namespace forms

#endif // FORMS_CHANNEL_OBJECT_WIDGET_H_
