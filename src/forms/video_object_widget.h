#ifndef FORMS_VIDEO_OBJECT_WIDGET_H_
#define FORMS_VIDEO_OBJECT_WIDGET_H_

#include <QWidget>
class IVideo;
class QByteArray;

namespace Ui
{
	class VideoObjectWidget;
}

namespace forms
{

class VideoObjectWidget : public QWidget
{
Q_OBJECT

public:
	VideoObjectWidget(IVideo & pObject, QWidget * parent);
	~VideoObjectWidget();

	inline IVideo & object() const { return mVideo; }
	inline IVideo & video() const { return mVideo; }

	void setLogo(QByteArray const & pData);
	bool operator< (VideoObjectWidget const & pOther) const;

public slots:
	void updateFromObject();

signals:
	void clicked(IVideo * pVideo = nullptr);

private slots:
	void on_btnVideo_clicked();

private:
	Ui::VideoObjectWidget * ui;
	IVideo & mVideo;
};

} // namespace forms

#endif // FORMS_VIDEO_OBJECT_WIDGET_H_
