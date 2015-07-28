#ifndef SERVICES_MAIN_H_
#define SERVICES_MAIN_H_

#define MAIN(cls,name) \
extern "C" \
{ \
Q_DECL_EXPORT IProfile * ServiceFactory() \
{ \
	return new cls(); \
} \
Q_DECL_EXPORT const char * ServiceName() \
{ \
	return name; \
} \
}

#endif // SERVICES_MAIN_H_
