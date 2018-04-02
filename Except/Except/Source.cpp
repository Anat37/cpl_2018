#include "Myexceptions.h"
#include<string>
#include<functional>

int clnum = 1;


class TestObject : public BaseObject {
public:
	int num;
	TestObject() {
		num = clnum++;
	}

	~TestObject() {
		std::cout << "destroyed " << num << std::endl;
	}
};

class MessageException : public BaseException {
public:
	std::string message;

	MessageException(std::string str) {
		message = str;
	}
};

class NumException : public BaseException {
public:
	int num;

	NumException(int c) {
		num = c;
	}
};

void fooThrowMessage(TestObject arg) {
	THROW(new MessageException("foo throw"))
}

void fooThrowNum(TestObject arg) {
	std::cout << "throwed exception" << std::endl;
	THROW(new NumException(5))
}

void foo(TestObject arg, std::function<void(TestObject)> func) {
	func(TestObject()); // 2
}

void fooWithMessageTry(TestObject arg, std::function<void(TestObject)> func) {
	TRY
		foo(TestObject(), func);  // 1
	CATCH(MessageException, exc)
		std::cout << exc.message << std::endl;
	}
}

void fooWithNumTry(TestObject arg, std::function<void(TestObject)> func) {
	TRY
		fooWithMessageTry(TestObject(), func);
	CATCH(NumException, exc)
		std::cout << exc.num << std::endl;
}
}


void test1() {
	fooWithNumTry(TestObject(), std::function<void(TestObject)>(fooThrowNum)); // 0
}

void test2() {
	fooWithMessageTry(TestObject(), std::function<void(TestObject)>(fooThrowMessage));
}

class BadTestObject : public BaseObject {
public:
	int num;
	BadTestObject() {
		num = clnum++;
	}

	~BadTestObject() {
		std::cout << "bad destroyed " << num << std::endl;
		THROW(new NumException(98))
	}
};

void test3() {
	TRY
		BadTestObject obj;
		THROW(new NumException(6))
	CATCH(NumException, p)
		std::cout << p.num << std::endl;
	}
	
}

int main() {
	TRY
		test1();
		test2();
		THROW(new BaseException())
	CATCH(BaseException, p)
		std::cout << "error";
	}
	//test3();
	system("pause");
}