#include "Myexceptions.h"

std::vector<jmp_buf*> envs;
std::vector<BaseObject*> stackObjects;
std::vector<int> tries;
int jmpVal = 0;
jmp_buf* env = nullptr;
BaseException* raisedException = nullptr;

BaseObject::BaseObject() {
	stackObjects.push_back(this);
}

BaseObject::~BaseObject() {
	stackObjects.pop_back();
	std::cout << "base object destruction" << std::endl;
}

void callDestroyers() {
	for (int i = stackObjects.size() - 1; i >= tries.back(); i--) {
		stackObjects[i]->~BaseObject();
	}
	tries.pop_back();
}
