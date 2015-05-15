#ifndef CORE_STORAGE_ACCESS_H_
#define CORE_STORAGE_ACCESS_H_

class QDir;

template <typename T> class Initializer;

class StorageAccess
{
public:
	inline StorageAccess() {}
	inline ~StorageAccess() {}

protected:
	QDir cache() const;

private:
	friend class Initializer<StorageAccess>;
	static void initialize();
	static void finalize();
	static void checkDatabaseStructure();
};

#endif // CORE_STORAGE_ACCESS_H_
