#ifndef SERVICES_MAIN_H_
#define SERVICES_MAIN_H_

#define MAIN(cls,name) \
extern "C" \
{ \
Profile * ServiceFactory() \
{ \
	return new cls(); \
} \
const char * ServiceName() \
{ \
	return name; \
} \
}

#endif // SERVICES_MAIN_H_
