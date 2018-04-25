#include"Header.h"

// ������� �����
VIRTUAL_CLASS(Base)
int a;
END(Base)
// ������


// �����-���������
VIRTUAL_CLASS_DERIVED(Derived, Base)
int b = 5;
END_DERIVE(Derived, Base)
// ������



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

	VIRTUAL_CALL((&base), Both); // �������� �Base::Both a = 0�
	VIRTUAL_CALL(reallyDerived, Both); // �������� �Derived::Both b = 1�
	VIRTUAL_CALL(reallyDerived, OnlyBase);  // �������� �Base::OnlyBase�
	VIRTUAL_CALL(reallyDerived, OnlyDerived);
	system("pause");
}
