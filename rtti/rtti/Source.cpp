#include"Header.h"

class A {
ENABLE_RTTI
public:
	A() { 
		RTTI_INIT(A)
		RTTI_END_INIT
	}
	int mya = 91;
};
END_ENABLE_RTTI(A)

class B {
ENABLE_RTTI
public:
	B(){ 
		RTTI_INIT(B)
		RTTI_END_INIT
	}
	int myb = 9;
};
END_ENABLE_RTTI(B)

class C :public A, public B {
ENABLE_RTTI
public:
	C() {
		RTTI_INIT(C)
		RTTI_INIT_ADD_BASE(C, A)
		RTTI_INIT_ADD_BASE(C, B)
		RTTI_END_INIT
	}
	int myc = 199;
};
END_ENABLE_RTTI(C)

class D : public C {
ENABLE_RTTI
public:
	D(){
		RTTI_INIT(D)
		RTTI_INIT_ADD_BASE(D, C)
		RTTI_END_INIT
	}
	int myd = 191;
};
END_ENABLE_RTTI(D)

class E : public C {
ENABLE_RTTI
public:
	E() { 
		RTTI_INIT(E)
		RTTI_INIT_ADD_BASE(E, C)
		RTTI_END_INIT
	}
	int mye = 190;
};
END_ENABLE_RTTI(E)

int main() {
	A* ptrD = new D();
	B* ptrB = new D();
	//C* ptrA = ptrD;
	//std::cout << ptrD->mytypeid << std::endl;
	//std::cout << (int)ptrA - (int)ptrD << std::endl;
	//ptrD->mytypeid;
	void* ptr = (void*)ptrB;
	std::cout << checkClass((void*)ptrD, 4) << std::endl;
	std::cout << findTop(ptr) << std::endl;
	std::cout << (DYNAMIC_CAST(ptrB, C))->myc << std::endl;
	std::cout << (DYNAMIC_CAST(ptrB, A))->mya << std::endl;
	std::cout << (MYTYPEID(ptrB) == MYTYPEID(ptrD)) << std::endl;
	system("pause");
}