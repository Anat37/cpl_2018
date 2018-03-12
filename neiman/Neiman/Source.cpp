#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h>

#include "Header.h"



#define IP memory[IP_ADD]

int memory[MEMORY_SIZE];

void if_t() {
	if (memory[REG_3]) {
		IP = memory[IP + 1];
	} else {
		IP = memory[IP + 2];
	}
}

void if_not() {
	if (!memory[REG_3]) {
		IP = memory[IP + 1];
	}
	else {
		IP = memory[IP + 2];
	}
}

void equal() {
	memory[REG_3] = memory[REG_1] == memory[REG_2];
	IP++;
}

void more() {
	memory[REG_3] = memory[REG_1] > memory[REG_2];
	IP++;
}

void and() {
	memory[REG_3] = memory[REG_1] && memory[REG_2];
	IP++;
}

void or() {
	memory[REG_3] = memory[REG_1] || memory[REG_2];
	IP++;
}
void move() {
	memory[memory[IP+2]] = memory[memory[IP + 1]];
	IP += 3;
}

void sum_reg() {
	memory[REG_3] = memory[REG_1] + memory[REG_2];
	IP += 1;
}

void dec_reg() {
	memory[REG_3] = memory[REG_1] - memory[REG_2];
	IP += 1;
}

void print_int_reg() {
	switch (memory[IP + 1])
	{
	case 1: std::cout << memory[REG_1]; break;
	case 2: std::cout << memory[REG_2]; break;
	case 3: std::cout << memory[REG_3]; break;
	}
	IP += 2;
}

void scan_int_reg() {
	switch (memory[IP + 1])
	{
	case 1: std::cin >> memory[REG_1]; break;
	case 2: std::cin >> memory[REG_2]; break;
	case 3: std::cin >> memory[REG_3]; break;
	}
	IP += 2;
}

void move_const() {
	memory[memory[IP + 2]] = memory[IP + 1];
	IP += 3;
}

void put_val() {
	memory[memory[SP_ADD]--] = memory[IP + 1];
	IP += 2;
}

void pop() {
	memory[SP_ADD]++;
	IP += 1;
}


void look_st() {
	memory[REG_4] = memory[memory[SP_ADD] + memory[IP + 1]];
	IP += 2;
}

void goto_add() {
	IP = memory[IP + 1];
}

void goto_from() {
	IP = memory[memory[IP + 1]];
}

void put_from() {
	memory[memory[SP_ADD]--] = memory[memory[IP + 1]];
	IP += 2;
}

void look_st_add() {
	memory[memory[IP + 2]] = memory[SP_ADD] + memory[IP + 1];
	IP += 3;
}

void print_string() {
	printf("%s", (char*)(memory + memory[IP + 1]));
}

void main() {
	
	FILE* f = fopen("input_code.bin", "r");
	//memory = new int[MEMORY_SIZE];
	int read_cnt = 0;
	/*while (read_cnt < MEMORY_SIZE) {
		int p = fread((void*)(memory+read_cnt), sizeof(int), MEMORY_SIZE - read_cnt, f);
		read_cnt += p;
	}*/
	fread((void*)(memory + read_cnt), sizeof(int), MEMORY_SIZE - read_cnt, f);
	while (true) {
		switch (memory[IP]) {
		case MOV: move(); break;
		case SUM_REG: sum_reg(); break;
		case DEC_REG: dec_reg(); break;
		case PRINT_INT_REG: print_int_reg(); break;
		case SCAN_INT_REG: scan_int_reg(); break;
		case IF_T: if_t(); break;
		case IF_NOT: if_not(); break;
		case EQUAL: equal(); break;
		case MORE: more(); break;
		case AND: and (); break;
		case OR: or (); break;
		case VARIABLE: IP += 2; break;
		case PUT_VAL: put_val(); break;
		case PUT_FROM: put_from(); break;
		case POP: pop(); break;
		case LOOK_ST: look_st(); break;
		case GOTO_ADD: goto_add(); break;
		case GOTO_FROM: goto_from(); break;
		case LOOK_ST_ADD: look_st_add(); break;
		case PRINT_STRING: print_string(); break;
		}

		if (memory[IP] == STOP_COMM)
			break;
	}
	system("pause");
	//delete memory;
	fclose(f);
}