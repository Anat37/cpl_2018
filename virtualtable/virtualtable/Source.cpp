#include"Header.h"

// базовый класс
VIRTUAL_CLASS(Base)
int a;
END(Base)
// методы


// класс-наследник
VIRTUAL_CLASS_DERIVED(Derived, Base)
int b = 5;
END_DERIVE(Derived, Base)
// методы



int main() {
	DECLARE_METHOD(Base, Both, std::cout << obj->a << std::endl;)
	DECLARE_METHOD(Base, OnlyBase)
	DECLARE_METHOD(Derived, Both, std::cout << obj->a << std::endl;)
	DECLARE_METHOD(Derived, OnlyDerived, std::cout << obj->b << std::endl;)
	Base base;
	base.a = 0;
	Derived derived;
	derived.a = 2;
	Base * reallyDerived = reinterpret_cast<Base*>(&derived);

	VIRTUAL_CALL((&base), Both); // печатает УBase::Both a = 0Ф
	VIRTUAL_CALL(reallyDerived, Both); // печатает УDerived::Both b = 1Ф
	VIRTUAL_CALL(reallyDerived, OnlyBase);  // печатает УBase::OnlyBaseФ
	VIRTUAL_CALL(reallyDerived, OnlyDerived);
	system("pause");
}
