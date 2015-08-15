#ifndef FORMS_MAIN_WINDOW_VIDEOS_H_
#define FORMS_MAIN_WINDOW_VIDEOS_H_

#include <QWidget>

class IChannel;
class IVideo;
template <typename T> class QList;

namespace Ui
{
	class MainWindowVideos;
}

namespace forms
{

class VideoObjectWidget;

class MainWindowVideos : public QWidget
{
Q_OBJECT

public:
	explicit MainWindowVideos(IChannel & pChannel, QWidget * parent);
	~MainWindowVideos();

public slots:
	void refresh();
	void rollup();

private slots:
	void checkRollup(int pSliderValue);
	void onVideosUpdated();
	void selected(IVideo * pVideo);

private:
	Ui::MainWindowVideos * ui;

	IChannel & mChannel;
	QList<VideoObjectWidget*> mVideos;
};

} // namespace forms

#endif // FORMS_MAIN_WINDOW_VIDEOS_H_
