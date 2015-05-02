#ifndef CORE_FIRST_TIME_H_
#define CORE_FIRST_TIME_H_

class FirstTime
{
public:
	inline FirstTime() : mValue(true) {}
	inline operator bool() { bool lOldValue = mValue; mValue = false; return lOldValue; }
	inline void reset() { mValue = true; }

private:
	bool mValue;
};

#endif // CORE_FIRST_TIME_H_
