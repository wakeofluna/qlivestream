#include "download_window.h"
#include "ui_download_window.h"

#include <qglobal.h>
#include <qnamespace.h>
#include <QByteArray>
#include <QDebug>
#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QObject>
#include <QProgressBar>
#include <QPushButton>
#include <QStandardPaths>
#include <QTimer>

#include "../core/i_downloader.h"

namespace forms
{

DownloadWindow::DownloadWindow(IDownloader * pDownloader) : QWidget(nullptr, Qt::Window), mDownloader(pDownloader)
{
	Q_ASSERT(mDownloader != nullptr);

	mPaused = false;
	mMerged = true; // TODO make configurable
	mTotalSize = 0;

	ui = new Ui::DownloadWindow();
	ui->setupUi(this);

	connect(mDownloader, &IDownloader::initialized, this, &DownloadWindow::onDownloaderInitialized);
	connect(mDownloader, &IDownloader::chunkStarted, this, &DownloadWindow::onChunkStarted);
	connect(mDownloader, &IDownloader::chunkProgress, this, &DownloadWindow::onChunkProgress);
	connect(mDownloader, &IDownloader::chunkCanceled, this, &DownloadWindow::onChunkCanceled);
	connect(mDownloader, &IDownloader::chunkCompleted, this, &DownloadWindow::onChunkCompleted);

	connect(this, &DownloadWindow::chunkFinished, this, &DownloadWindow::downloadNext);

	ui->btnBox->button(QDialogButtonBox::Abort)->setText(tr("Pause"));
	ui->btnBox->button(QDialogButtonBox::Retry)->setText(tr("Resume"));

	ui->prgFile->hide();
	ui->prgTotal->hide();
	ui->btnBox->button(QDialogButtonBox::Open)->setEnabled(false);
	ui->btnBox->button(QDialogButtonBox::Close)->hide();
	ui->btnBox->button(QDialogButtonBox::Abort)->setEnabled(false);
	ui->btnBox->button(QDialogButtonBox::Retry)->hide();
	ui->btnBox->button(QDialogButtonBox::Cancel)->setEnabled(false);

	ui->lblStatus->setText(tr("Initializing download..."));
	mDownloader->initialize();
}

DownloadWindow::~DownloadWindow()
{
	if (mPlaylist.isOpen())
	{
		mPlaylist.write("#EXT-X-ENDLIST\n");
		mPlaylist.close();
	}

	delete ui;
	delete mDownloader;
}

void DownloadWindow::pause()
{
	ui->btnBox->button(QDialogButtonBox::Abort)->setEnabled(false);
	mPaused = true;
}

void DownloadWindow::resume()
{
	ui->btnBox->button(QDialogButtonBox::Retry)->setEnabled(false);
	mPaused = false;
	downloadNext();
}

void DownloadWindow::cancel()
{
	close();
}

void DownloadWindow::downloadNext()
{
	if (mPaused)
	{
		ui->btnBox->button(QDialogButtonBox::Abort)->hide();
		ui->btnBox->button(QDialogButtonBox::Retry)->setEnabled(true);
		ui->btnBox->button(QDialogButtonBox::Retry)->show();
		ui->btnBox->button(QDialogButtonBox::Cancel)->setEnabled(true);

		if (mPlaylist.isOpen())
			mPlaylist.flush();

		if (mStatusFile.isOpen())
			mStatusFile.flush();

		if (mCurrentFile.isOpen())
			mCurrentFile.flush();

		return;
	}

	int lNext = mDownloader->currentChunk() + 1;

	if (lNext < mDownloader->numChunks())
	{
		ui->lblStatus->setText(tr("Downloading chunk %1").arg(lNext+1));

		ui->prgTotal->setValue(lNext);
		ui->prgFile->setMinimum(0);
		ui->prgFile->setMaximum(0);
		ui->prgFile->setValue(0);

		mDownloader->setCurrentChunk(lNext);
		mCurrentData.clear();

		if (!mMerged)
		{
			QString lFullname = mTarget.filePath(mDownloader->suggestedFilename(lNext));
			mCurrentFile.close();
			mCurrentFile.setFileName(lFullname);
		}

		if (!mMerged && mCurrentFile.exists())
		{
			ui->prgFile->setValue(1);
			ui->prgFile->setMaximum(1);
			onChunkCompleted();
		}
		else
		{
			QTimer::singleShot(200, [this] { mDownloader->startDownload(); });

			ui->btnBox->button(QDialogButtonBox::Abort)->setEnabled(lNext < mDownloader->numChunks() - 1);
			ui->btnBox->button(QDialogButtonBox::Abort)->show();
			ui->btnBox->button(QDialogButtonBox::Retry)->hide();
			ui->btnBox->button(QDialogButtonBox::Cancel)->setEnabled(false);
		}
	}
	else
	{
		ui->lblStatus->setText(tr("Downloading finished"));

		ui->btnBox->button(QDialogButtonBox::Abort)->setEnabled(false);
		ui->btnBox->button(QDialogButtonBox::Cancel)->hide();
		ui->btnBox->button(QDialogButtonBox::Close)->show();

		if (mPlaylist.isOpen())
		{
			mPlaylist.write("#EXT-X-ENDLIST\n");
			mPlaylist.close();
		}

		if (mCurrentFile.isOpen())
			mCurrentFile.close();

		if (mStatusFile.isOpen())
			mStatusFile.close();

		emit finished();
	}
}

void DownloadWindow::on_btnBox_clicked(QAbstractButton * pButton)
{
	switch (ui->btnBox->standardButton(pButton))
	{
		case QDialogButtonBox::Open:
			QDesktopServices::openUrl(QString("file://%1").arg(mTarget.path()));
			break;

		case QDialogButtonBox::Retry:
			resume();
			break;

		case QDialogButtonBox::Abort:
			pause();
			break;

		case QDialogButtonBox::Close:
			close();
			break;

		case QDialogButtonBox::Cancel:
			cancel();
			break;

		default:
			Q_UNREACHABLE();
			break;
	}
}

void DownloadWindow::onDownloaderInitialized()
{
	if (mDownloader->numChunks() < 1)
	{
		qWarning() << "Download initialization failed";
		close();
		return;
	}

	ui->prgTotal->setMaximum(mDownloader->numChunks());
	ui->prgTotal->show();
	ui->prgFile->show();

	if (mDownloader->numChunks() == 1)
		ui->btnBox->button(QDialogButtonBox::Abort)->hide();

	QString lDir = QFileDialog::getExistingDirectory(this, QString(), QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
	if (lDir.isEmpty())
	{
		close();
		return;
	}

	mTarget = lDir;

	if (mDownloader->numChunks() > 1 && !mMerged)
	{
		QString lSubdir = mDownloader->suggestedFolder();
		if (!lSubdir.isEmpty())
		{
			mTarget.mkdir(lSubdir);
			mTarget.cd(lSubdir);
			mPlaylist.setFileName(mTarget.filePath(QString("%1.m3u").arg(mDownloader->suggestedFilename(-1))));
		}
	}
	else
	{
		mCurrentFile.setFileName(mTarget.filePath(mDownloader->suggestedFilename(-2)));
		mStatusFile.setFileName(mTarget.filePath(QString("%1.log").arg(mDownloader->suggestedFilename(-2))));

		if (mStatusFile.exists())
		{
			mStatusFile.open(QFile::ReadOnly);

			qint64 lLastChunk;
			qint64 lFileSize;
			QTextStream lStatus(&mStatusFile);
			lStatus >> lLastChunk >> lFileSize;

			mDownloader->setCurrentChunk(lLastChunk);
			mTotalSize = lFileSize;

			mStatusFile.close();
		}

		if (mCurrentFile.exists())
		{
			mCurrentFile.open(QFile::WriteOnly | QFile::Append);
			mCurrentFile.resize(mTotalSize);
		}
	}

	mCurrentData.reserve(12*1024*1024);

	ui->btnBox->button(QDialogButtonBox::Open)->setEnabled(true);
	ui->btnBox->button(QDialogButtonBox::Cancel)->setEnabled(true);
	downloadNext();
}

void DownloadWindow::onChunkStarted()
{
	int lSize = mDownloader->currentChunkSize();
	if (lSize == -1)
		ui->prgFile->setMaximum(1024);
	else
		ui->prgFile->setMaximum(lSize);
}

void DownloadWindow::onChunkProgress(QByteArray pData)
{
	if (mCurrentData.size() + pData.size() > mCurrentData.capacity())
	{
		if (!mCurrentFile.isOpen())
			mCurrentFile.open(QFile::Append);

		mCurrentFile.write(mCurrentData);
		mCurrentData.clear();
	}

	mCurrentData.append(pData);

	int lProgress = mDownloader->currentChunkProgress();
	if (ui->prgFile->maximum() < lProgress)
		ui->prgFile->setMaximum(lProgress * 2);
	ui->prgFile->setValue(lProgress);
}

void DownloadWindow::onChunkCanceled()
{
	mCurrentData.clear();
	ui->prgFile->setValue(-1);
	ui->btnBox->button(QDialogButtonBox::Cancel)->hide();
	ui->btnBox->button(QDialogButtonBox::Close)->show();
	close();
}

void DownloadWindow::onChunkCompleted()
{
	int lCurrent = mDownloader->currentChunk();

	ui->prgFile->setMaximum(ui->prgFile->value());
	ui->prgTotal->setValue(lCurrent + 1);

	if (!mCurrentData.isEmpty())
	{
		if (!mCurrentFile.isOpen())
			mCurrentFile.open(QFile::Append);

		mCurrentFile.write(mCurrentData);
		mCurrentData.clear();
	}

	mTotalSize += mDownloader->currentChunkProgress();

	if (!mPlaylist.fileName().isEmpty())
	{
		if (!mPlaylist.isOpen())
		{
			mPlaylist.open(QFile::Truncate | QFile::WriteOnly);
			mPlaylist.write("#EXTM3U\n#EXT-X-VERSION:3\n#EXT-X-TARGETDURATION:2\n\n");
		}

		double lLength = mDownloader->currentChunkLength();
		if (lLength > 0.001)
		{
			QString lInf = QString("#EXTINF:%1,\n").arg(lLength, 0, 'f', 3);
			mPlaylist.write(lInf.toLatin1());
		}

		QString lItem = mCurrentFile.fileName();
		int lSlash = lItem.lastIndexOf('/');
		if (lSlash != -1)
			lItem = lItem.mid(lSlash+1);

		mPlaylist.write(lItem.toLatin1());
		mPlaylist.write("\n");
	}

	if (!mStatusFile.fileName().isEmpty())
	{
		if (!mStatusFile.isOpen())
		{
			mStatusFile.open(QFile::WriteOnly | QFile::Truncate);
		}
		else
		{
			mStatusFile.resize(0);
			mStatusFile.seek(0);
		}

		QString lStatus = QString("%1 %2").arg(mDownloader->currentChunk()).arg(mTotalSize);
		mStatusFile.write(lStatus.toUtf8());
	}

	emit chunkFinished();
}



} // namespace forms
