#ifndef FORMS_DOWNLOAD_WINDOW_H_
#define FORMS_DOWNLOAD_WINDOW_H_

#include <QWidget>
#include <QDir>
#include <QFile>

class IDownloader;
class IVideo;
class QAbstractButton;

namespace Ui
{
	class DownloadWindow;
}

namespace forms
{

class DownloadWindow : public QWidget
{
Q_OBJECT

public:
	DownloadWindow(IDownloader * pDownloader);
	~DownloadWindow();

public slots:
	void pause();
	void resume();
	void cancel();
	void downloadNext();

signals:
	void chunkFinished();
	void finished();

private slots:
	void on_btnBox_clicked(QAbstractButton * pButton);
	void onDownloaderInitialized();
	void onChunkStarted();
	void onChunkProgress(QByteArray pData);
	void onChunkCanceled();
	void onChunkCompleted();

private:
	Ui::DownloadWindow * ui;

	IDownloader * mDownloader;
	bool mPaused;
	bool mMerged;
	QDir mTarget;
	QByteArray mCurrentData;
	QFile mCurrentFile;
	QFile mStatusFile;
	QFile mPlaylist;
	qint64 mTotalSize;
};

} // namespace forms

#endif // FORMS_DOWNLOAD_WINDOW_H_
