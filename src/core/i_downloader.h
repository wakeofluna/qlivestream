#ifndef CORE_I_DOWNLOADER_H_
#define CORE_I_DOWNLOADER_H_

#include <QByteArray>
#include <QObject>
#include "network_access.h"

#include "../config.h"

class COREDLL IDownloader : public QObject, protected NetworkAccess
{
Q_OBJECT

public:
	~IDownloader();

	inline int currentChunk() const { return mCurrentChunk; }
	inline int currentChunkSize() const { return mChunkSize; }
	inline int currentChunkProgress() const { return mChunkProgress; }
	inline double currentChunkLength() const { return mChunkLength; }

	virtual QString suggestedFolder() const = 0;
	virtual QString suggestedFilename(int pChunkNr) const = 0;
	virtual void initialize() = 0;
	virtual int numChunks() const = 0;
	virtual void setCurrentChunk(int pChunkNr) = 0;
	virtual void startDownload() = 0;
	virtual void cancelDownload() = 0;

signals:
	void initialized();
	void chunkStarted();
	void chunkProgress(QByteArray pData);
	void chunkCompleted();
	void chunkCanceled();

protected:
	IDownloader(QObject * parent = nullptr);

	int mCurrentChunk;
	int mChunkSize;
	int mChunkProgress;
	double mChunkLength;
};

#endif // CORE_I_DOWNLOADER_H_
