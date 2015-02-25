#include <stdio.h>
#include <string.h>

#include "bytecode.h"

int main(int argc, char *argv[])
{
	printf("generating bytecode...\n");
	FILE *fp;
	fp = fopen("code", "wb");
	if(fp == NULL) {
		printf("could not open gencode for writing");
	} else {
		uint16_t instructions[] = {
		BC_FN_DEF, 0, 0, 0,
		BC_LOAD_CONSTANT, REG_A, 11, 0,
		BC_PRINT_NUM, REG_A, 0, 0,
		BC_CALL, 1, 0, 0,
		BC_RET, 0, 0, 0,
		BC_FN_DEF, 1, 0, 0,
		BC_LOAD_CONSTANT, REG_A, 12, 0,
		BC_LOAD_CONSTANT, REG_B, 12, 0,
		BC_CMP, REG_A, REG_B, 0,
		BC_REL_JE, 1, 0, 0,
		BC_LOAD_CONSTANT, REG_A, 50, 0,
		BC_PRINT_NUM, REG_A, 0, 0,
		BC_PRINT_STR, 5, 0, 0,
		BC_RET, 0, 0, 0,
		BC_BEGIN_DATA, 0, 0, 0,
		};
		const char data[] = "\x00hello world\0\x01second string\0\x05this id is 5\n\0";

		fwrite(instructions, sizeof(uint16_t), sizeof(instructions)/sizeof(uint16_t), fp);
		fwrite(data, sizeof(char), sizeof(data)/sizeof(char), fp);

		fclose(fp);
		printf("OK\n");
	}
}
