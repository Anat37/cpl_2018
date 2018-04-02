#pragma once
#include<vector>
#include<setjmp.h>
#include <iostream>


class BaseObject {
public:
	BaseObject();
	virtual ~BaseObject();
};

class BaseException {
public:
	virtual ~BaseException() {};
};

extern std::vector<jmp_buf*> envs;
extern std::vector<BaseObject*> stackObjects;
extern std::vector<int> tries;
extern BaseException* raisedException;
extern int jmpVal;
extern jmp_buf* env;
void callDestroyers();

#define TRY tries.push_back(stackObjects.size());   \
		env = new jmp_buf[1];						\
		jmpVal = setjmp(*env);						\
		if (!jmpVal) {								\
			envs.push_back(env);

#define RETHROW(exception) if (envs.size() == 0) {	\
								exit(1);			\
							}						\
							raisedException = exception; \
							callDestroyers();		\
							longjmp(*envs.back(), 1);


#define THROW(exception) if (raisedException != nullptr) {	\
							exit(1);						\
						} else {							\
							RETHROW(exception)				\
						}	

#define CATCH(type, name)		delete envs.back();								\
								envs.pop_back();								\
							} else {											\
								delete envs.back();								\
								envs.pop_back();								\
								type* ptr = dynamic_cast<type*>(raisedException); \
								if (ptr == nullptr) {							\
									RETHROW(raisedException)					\
								} else {										\
									raisedException = nullptr;					\
								}												\
								type name = *ptr;





