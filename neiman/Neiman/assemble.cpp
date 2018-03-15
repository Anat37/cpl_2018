#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <set>
#include <vector>
#include "Header.h"


int memory[MEMORY_SIZE];

struct variableDefinition {
	variableDefinition() {}

	variableDefinition(std::string _name) {
		name = _name;
	}

	int address;
	std::string name;
	int value = 0;

	bool operator ()(const variableDefinition& a, const variableDefinition& b) const {
		return a.name > b.name;
	}
};

struct functionDefinition {
	functionDefinition(){}

	functionDefinition(std::string _name) {
		name = _name;
	}

	int address = 0;
	int arg_cnt = 0;
	std::string name;
	std::vector<int> code;
	std::vector<variableDefinition> variables;
	bool rec = false;

	bool operator ()(const functionDefinition& a, const functionDefinition& b) const {
		return a.name > b.name;
	}
};

std::set<variableDefinition, variableDefinition> variables;
std::set<functionDefinition, functionDefinition> functions;
std::vector<int> ifPointersStack;
int currentVariablePtr = GL_VAR;
int currentFunctionPtr = 0;
int currentStringPtr = STR_ADD;

int resolveRegister(const std::string& str) {
	if (str == "reg1") {
		return 1;
	}
	if (str == "reg2") {
		return 2;
	}
	if (str == "reg3") {
		return 3;
	}
	return -1;
}

int resolveArguments(functionDefinition& foo, const std::string& str, int stackOffset) {
	if (str == "reg1") {
		return REG_1;
	}
	if (str == "reg2") {
		return REG_2;
	}
	if (str == "reg3") {
		return REG_3;
	}
	auto var1 = variables.find(variableDefinition(str));
	if (var1 != variables.end()) {
		return var1->address;
	}
	for (int i = 0; i < foo.variables.size(); ++i) {
		if (foo.variables[i].name == str) {
			foo.code.push_back(LOOK_ST);
			foo.code.push_back(foo.variables.size() - i + stackOffset);
			return REG_4;
		}
	}
	return -1;
}

int resolveArgumentsForDestination(functionDefinition& foo, std::string& str, int& indin) {
	if (str == "reg1") {
		return REG_1;
	}
	if (str == "reg2") {
		return REG_2;
	}
	if (str == "reg3") {
		return REG_3;
	}
	auto var1 = variables.find(variableDefinition(str));
	if (var1 != variables.end()) {
		return var1->address;
	}
	for (int i = 0; i < foo.variables.size(); ++i) {
		if (foo.variables[i].name == str) {
			foo.code.push_back(LOOK_ST_ADD);
			foo.code.push_back(foo.variables.size() - i);
			indin = foo.code.size();
			foo.code.push_back(1);
			return -1;
		}
	}
}

variableDefinition createVariableDefinition() {
	std::string str;
	variableDefinition var1;
	std::cin >> str;
	var1.name = str;
	std::cin >> str;
	var1.value = std::stoi(str);
	return var1;
}

void addGlobalVariable() {
	variableDefinition var1 = createVariableDefinition();
	var1.address = currentVariablePtr--;
	variables.insert(var1);
}

void addLocalVariable(functionDefinition& foo) {
	variableDefinition var1 = createVariableDefinition();
	foo.code.push_back(PUT_VAL);
	foo.code.push_back(var1.value);
	foo.variables.push_back(var1);
}

void addGlobalString() {
	std::string name, token, str;
	std::cin >> name >> token;
	while (token.back() != '\"') {
		str.append(token + ' ');
		std::cin >> token;
	}
	str.append(token + '\0');
	variableDefinition var;
	var.name = name;
	var.value = currentStringPtr;
	strcpy((char*)(memory + currentStringPtr), str.c_str());
	currentStringPtr += str.size() + 1;
	var.address = currentVariablePtr--;
	variables.insert(var);
}

void addPrintsFunction(functionDefinition& foo) {
	std::string arg1;
	std::cin >> arg1;
	int arga = resolveArguments(foo, arg1, 0);
	foo.code.push_back(PRINT_STRING);
	foo.code.push_back(arga);
}

void addMoveFunction(functionDefinition& foo) {
	std::string arg1, arg2;
	std::cin >> arg1 >> arg2;
	int arga, argb;
	arga = resolveArguments(foo, arg1, 0);
	int indin;
	argb = resolveArgumentsForDestination(foo, arg2, indin);
	if (arga == -1) {
		foo.code.push_back(VARIABLE);
		foo.code.push_back(std::stoi(arg1));
		arga = foo.address + foo.code.size() - 1;
	}
	foo.code.push_back(MOV);
	foo.code.push_back(arga);
	if (argb == -1) {
		foo.code[indin] = foo.code.size() + foo.address;
		foo.code.push_back(1);
	} else {
		foo.code.push_back(argb);
	}
}

void addFunctionCall(functionDefinition& foo) {
	std::string str;
	std::cin >> str;
	functionDefinition foo2;
	if (foo.rec && foo.name == str) {
		foo2 = foo;
	} else {
		foo2 = *functions.find(functionDefinition(str));
	}
	// адрес возврата
	foo.code.push_back(PUT_VAL);
	foo.code.push_back(REG_3);
	int ind_sum = foo.code.size() - 1;
	//аргументы
	for (int i = 0; i < foo2.arg_cnt; ++i) {
		std::cin >> str;
		int a = resolveArguments(foo, str, 1);
		if (a == -1) {
			foo.code.push_back(PUT_VAL);
			foo.code.push_back(std::stoi(str));
		} else {
			foo.code.push_back(PUT_FROM);
			foo.code.push_back(a);
		}
	}

	foo.code.push_back(GOTO_ADD);
	foo.code.push_back(foo2.address);
	foo.code[ind_sum] = foo.address + foo.code.size();
}

void addFunctionReturn(functionDefinition& foo) {
	for (int i = 0; i < foo.variables.size(); ++i) {
		foo.code.push_back(POP);
	}
	foo.code.push_back(LOOK_ST);
	foo.code.push_back(1);
	foo.code.push_back(POP);
	foo.code.push_back(GOTO_FROM);
	foo.code.push_back(REG_4);
}

void addPrintFunction(functionDefinition& foo) {
	std::string arg1;
	std::cin >> arg1;
	int arga = resolveRegister(arg1);
	foo.code.push_back(PRINT_INT_REG);
	foo.code.push_back(arga);
}

void startIf(functionDefinition& foo) {
	foo.code.push_back(IF_T);
	foo.code.push_back(foo.address + foo.code.size() + 2);
	foo.code.push_back(1);
	ifPointersStack.push_back(foo.code.size() - 1);
}

void addElse(functionDefinition& foo) {
	int ind = ifPointersStack.back();
	ifPointersStack.pop_back();
	foo.code.push_back(GOTO_ADD);
	foo.code.push_back(1);
	ifPointersStack.push_back(foo.code.size() - 1);
	foo.code[ind] = foo.code.size() + foo.address;
}

void addScanFunction(functionDefinition& foo) {
	std::string arg1;
	std::cin >> arg1;
	int arga = resolveRegister(arg1);
	foo.code.push_back(SCAN_INT_REG);
	foo.code.push_back(arga);
}

void endIf(functionDefinition& foo) {
	int ind = ifPointersStack.back();
	ifPointersStack.pop_back();
	foo.code[ind] = foo.code.size() + foo.address;
}

void processFunctionCode(functionDefinition& foo) {
	std::string str;
	while (std::cin >> str) {
		if (str == "gl_var") {
			addGlobalVariable();
		}
		if (str == "var") {
			addLocalVariable(foo);
		}
		if (str == "var_string") {
			addGlobalString();
		}
		if (str == "prints") {
			addPrintsFunction(foo);
		}
		if (str == "mov") {
			addMoveFunction(foo);
		}
		if (str == "sum") {
			foo.code.push_back(SUM_REG);	
		}
		if (str == "dec") {
			foo.code.push_back(DEC_REG);
		}
		if (str == "more") {
			foo.code.push_back(MORE);
		}
		if (str == "equal") {
			foo.code.push_back(EQUAL);
		}
		if (str == "or") {
			foo.code.push_back(OR);
		}
		if (str == "and") {
			foo.code.push_back(AND);
		}
		if (str == "call") {
			addFunctionCall(foo);
		}
		if (str == "return") {
			addFunctionReturn(foo);
		}
		if (str == "exit") {
			foo.code.push_back(STOP_COMM);
		}
		if (str == "print") {
			addPrintFunction(foo);
		}
		if (str == "if") {
			startIf(foo);
		}
		if (str == "else") {
			addElse(foo);
		}
		if (str == "endif") {
			endIf(foo);
		}
		if (str == "scan") {
			addScanFunction(foo);
		}
		if (str == "enddef") {
			return;
		}
	}
}

functionDefinition createFunctionDefinition() {
	std::string name;
	int num_arg;
	std::cin >> name;
	functionDefinition foo;
	if (name == "rec") {
		std::cin >> name;
		foo.name = name;
		foo.rec = true;
	}
	else {
		foo.name = name;
	}
	std::cin >> num_arg;
	foo.arg_cnt = num_arg;
	for (int i = 0; i < num_arg; ++i) {
		std::cin >> name;
		foo.variables.push_back(name);
	}
	foo.address = currentFunctionPtr;
	return foo;
}

int main() {
	FILE* output = fopen("machine_code.bin", "w");
	freopen("input_code.txt", "r", stdin);

	for (int i = 0; i < MEMORY_SIZE; ++i)
		memory[i] = 0;
	
	
	std::string str;
	std::cin >> str;
	while (str == "def") {
		functionDefinition foo = createFunctionDefinition();
		processFunctionCode(foo);
		currentFunctionPtr += foo.code.size();
		for (int i = foo.address; i < foo.code.size() + foo.address; ++i) {
			memory[i] = foo.code[i - foo.address];
		}
		functions.insert(foo);

		if (!(std::cin >> str))
			break;
	}
	for (variableDefinition var1: variables) {
		memory[var1.address] = var1.value;
	}
	memory[SP_ADD] = currentVariablePtr;
	auto foo2 = functions.find(functionDefinition("main"));
	memory[IP_ADD] = foo2->address;

	int write_cnt = 0;
	while (write_cnt < MEMORY_SIZE) {
		write_cnt += fwrite(memory, sizeof(int), MEMORY_SIZE - write_cnt, output);
	}
	std::fclose(output);
}