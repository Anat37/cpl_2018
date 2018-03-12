#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <iostream>
#include <string>
#include <set>
#include <vector>
#include "Header.h"


int memory[MEMORY_SIZE];

struct var {
	var() {}

	var(std::string _name) {
		name = _name;
	}

	int address;
	std::string name;
	int value = 0;

	bool operator ()(const var& a, const var& b) const {
		return a.name > b.name;
	}
};

struct fuckxutility {
	fuckxutility(){}

	fuckxutility(std::string _name) {
		name = _name;
	}

	int address = 0;
	int arg_cnt = 0;
	std::string name;
	std::vector<int> code;
	std::vector<var> variables;
	bool rec = false;

	
};

struct mycomp {
	bool operator ()(const fuckxutility& a, const fuckxutility& b) const {
		return a.name > b.name;
	}
};


std::set<var, var> variables;
std::set<fuckxutility, mycomp> functions;
std::vector<int> ifstack;
int curr_var_ptr = GL_VAR;
int curr_func_ptr = 0;
int curr_str_ptr = STR_ADD;

int resolve_arg(fuckxutility& foo, std::string& str) {
	if (str == "reg1") {
		return REG_1;
	}
	if (str == "reg2") {
		return REG_2;
	}
	if (str == "reg3") {
		return REG_3;
	}
	auto var1 = variables.find(var(str));
	if (var1 != variables.end()) {
		return var1->address;
	}
	for (int i = 0; i < foo.variables.size(); ++i) {
		if (foo.variables[i].name == str) {
			foo.code.push_back(LOOK_ST);
			foo.code.push_back(foo.variables.size() - i);
			return REG_4;
		}
	}
	return -1;
}

int resolve_arg_call(fuckxutility& foo, std::string& str) {
	if (str == "reg1") {
		return REG_1;
	}
	if (str == "reg2") {
		return REG_2;
	}
	if (str == "reg3") {
		return REG_3;
	}
	auto var1 = variables.find(var(str));
	if (var1 != variables.end()) {
		return var1->address;
	}
	for (int i = 0; i < foo.variables.size(); ++i) {
		if (foo.variables[i].name == str) {
			foo.code.push_back(LOOK_ST);
			foo.code.push_back(foo.variables.size() - i + 1);
			return REG_4;
		}
	}
	return -1;
}

int resolve_arg_dest(fuckxutility& foo, std::string& str, int& indin) {
	if (str == "reg1") {
		return REG_1;
	}
	if (str == "reg2") {
		return REG_2;
	}
	if (str == "reg3") {
		return REG_3;
	}
	auto var1 = variables.find(var(str));
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

int resolve_reg(fuckxutility& foo, std::string& str) {
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

var add_var() {
	std::string str;
	var var1;
	std::cin >> str;
	var1.name = str;
	std::cin >> str;
	var1.value = std::stoi(str);
	return var1;
}

void add_gl_var() {
	var var1 = add_var();
	var1.address = curr_var_ptr--;
	variables.insert(var1);
}

std::vector<std::string> tokens;



void add_func(fuckxutility& foo) {
	std::string str;
	while (std::cin >> str) {
		tokens.push_back(str);
		if (str == "gl_var") {
			add_var();
			continue;
		}
		if (str == "var") {
			var var1 = add_var();
			foo.code.push_back(PUT_VAL);
			foo.code.push_back(var1.value);
			foo.variables.push_back(var1);
		}
		if (str == "var_string") {
			std::string name, token, str1;
			std::cin >> name >> token;
			while (token.back() != '\"') {
				str1.append(token + ' ');
				std::cin >> token;
			}
			str1.append(token + '\0');
			var var1;
			var1.name = name;
			var1.value = curr_str_ptr;
			strcpy((char*)(memory + curr_str_ptr), str1.c_str());
			curr_str_ptr += str1.size() + 1;
			var1.address = curr_var_ptr--;
			variables.insert(var1);
		}
		if (str == "prints") {
			std::string arg1;
			std::cin >> arg1;
			int arga = resolve_arg(foo, arg1);
			foo.code.push_back(PRINT_STRING);
			foo.code.push_back(arga);
		}
		if (str == "mov") {
			std::string arg1, arg2;
			std::cin >> arg1 >> arg2;
			int arga, argb;
			arga = resolve_arg(foo, arg1);
			int indin;
			argb = resolve_arg_dest(foo, arg2, indin);
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
			std::cin >> str;
			fuckxutility foo2;
			if (foo.rec && foo.name == str) {
				foo2 = foo;
			} else {
				foo2 = *functions.find(fuckxutility(str));
			}
			// адрес возврата
			foo.code.push_back(PUT_VAL);
			foo.code.push_back(REG_3);
			int ind_sum = foo.code.size() - 1;
			//аргументы
			for (int i = 0; i < foo2.arg_cnt; ++i) {
				std::cin >> str;
				int a = resolve_arg_call(foo, str);
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
		if (str == "return") {
			for (int i = 0; i < foo.variables.size(); ++i) {
				foo.code.push_back(POP);
			}
			foo.code.push_back(LOOK_ST);
			foo.code.push_back(1);
			foo.code.push_back(POP);
			foo.code.push_back(GOTO_FROM);
			foo.code.push_back(REG_4);
		}
		if (str == "exit") {
			foo.code.push_back(STOP_COMM);
		}
		if (str == "print") {
			std::string arg1;
			std::cin >> arg1;
			int arga = resolve_reg(foo, arg1);
			foo.code.push_back(PRINT_INT_REG);
			foo.code.push_back(arga);
		}
		if (str == "if") {
			foo.code.push_back(IF_T);
			foo.code.push_back(foo.address + foo.code.size() + 2);
			foo.code.push_back(1);
			ifstack.push_back(foo.code.size() - 1);
		}
		if (str == "else") {
			int ind = ifstack.back();
			ifstack.pop_back();
			foo.code.push_back(GOTO_ADD);
			foo.code.push_back(1);
			ifstack.push_back(foo.code.size() - 1);
			foo.code[ind] = foo.code.size() + foo.address;
		}
		if (str == "endif") {
			int ind = ifstack.back();
			ifstack.pop_back();
			foo.code[ind] = foo.code.size() + foo.address;
		}
		if (str == "scan") {
			std::string arg1;
			std::cin >> arg1;
			int arga = resolve_reg(foo, arg1);
			foo.code.push_back(SCAN_INT_REG);
			foo.code.push_back(arga);
		}
		if (str == "enddef") {
			return;
		}
	}
}

int main() {
	FILE* output = fopen("machine_code.bin", "w");
	freopen("input_code.txt", "r", stdin);

	for (int i = 0; i < MEMORY_SIZE; ++i)
		memory[i] = 0;
	
	
	std::string str;
	std::cin >> str;
	int cnt = 0;
	while (str == "def") {
		cnt++;
		std::string name;
		int num_arg;
		std::cin >> name;
		fuckxutility foo;
		if (name == "rec") {
			std::cin >> name;
			foo.name = name;
			foo.rec = true;
		} else {
			foo.name = name;
		}
		std::cin >> num_arg;
		foo.arg_cnt = num_arg;
		for (int i = 0; i < num_arg; ++i) {
			std::cin >> name;
			foo.variables.push_back(name);
		}
		foo.address = curr_func_ptr;
		add_func(foo);
		curr_func_ptr += foo.code.size();
		for (int i = foo.address; i < foo.code.size() + foo.address; ++i) {
			memory[i] = foo.code[i - foo.address];
		}
		functions.insert(foo);

		if (!(std::cin >> str))
			break;
	}
	for (var var1: variables) {
		memory[var1.address] = var1.value;
	}
	memory[SP_ADD] = curr_var_ptr;
	auto foo2 = functions.find(fuckxutility("main"));
	memory[IP_ADD] = foo2->address;

	int write_cnt = 0;
	while (write_cnt < MEMORY_SIZE) {
		write_cnt += fwrite(memory, sizeof(int), MEMORY_SIZE - write_cnt, output);
	}
	std::fclose(output);
}