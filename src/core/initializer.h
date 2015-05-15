#ifndef CORE_INITIALIZER_H_
#define CORE_INITIALIZER_H_

template <typename T>
class Initializer
{
public:
	inline Initializer(bool pNow = true) : mInitialized(false) { if (pNow) run(); }
	inline ~Initializer() { T::finalize(); }
	inline void run() { if (!mInitialized) { mInitialized = true; T::initialize(); } }
	inline operator bool() const { return mInitialized; }
private:
	bool mInitialized;
};


#endif // CORE_INITIALIZER_H_
