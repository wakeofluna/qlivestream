#ifndef CORE_STORAGE_ACCESS_H_
#define CORE_STORAGE_ACCESS_H_

#include "config.h"
#include <functional>
class QByteArray;
class QString;

template <typename T> class Initializer;

class COREDLL StorageAccess
{
public:
	typedef std::function<void (QByteArray const&)> DataCallback;
	typedef std::function<void (DataCallback &&)> CacheMissCallback;

public:
	inline StorageAccess() {}
	inline ~StorageAccess() {}

protected:
	void accessCache(QString const & pCacheString, CacheMissCallback && pMiss, DataCallback && pHit);

private:
	friend class Initializer<StorageAccess>;
	static void initialize();
	static void finalize();
	static void checkDatabaseStructure();

	bool fromCache(QString const & pCacheString, QByteArray & pData) const;
	void insertInCache(QString const & pCacheString, QByteArray const & pData);
};

#endif // CORE_STORAGE_ACCESS_H_
