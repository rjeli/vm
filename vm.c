#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "bytecode.h"

struct fn_node {
	// id 0 is the main function
	uint16_t id;
	// number of instructions
	int ins_len;
	uint16_t *code;
	struct fn_node *next;
};

struct str_node {
	uint16_t id;
	char *string;
	struct str_node *next;
};

struct vm_env {
	uint16_t reg_a;
	uint16_t reg_b;
	uint16_t reg_c;
	uint16_t reg_d;
	uint16_t cmp_flag;
	struct str_node str_root;
};

void fatal(const char *msg)
{
	printf("fatal error: %s\n", msg);
	exit(1);
}

// load bytecode from file into memory and return a pointer into
// the function list
struct fn_node load_bytecode(FILE *fp, struct vm_env *env)
{
	uint16_t buf[4] = {0,0,0,0};
	char strbuf[64];
	int strbufi;
	char stridbuf;
	bool ret_reached;
	struct fn_node fn_root;
	struct fn_node *fn_i;
	bool first_fn_exists;

	printf("loading bytecode\n");

	while(true) {
		fread(buf, sizeof(uint16_t), 4, fp);

		if(buf[0] == BC_FN_DEF) {
			// set fn_i to a new function at end of list
			if(!first_fn_exists) {
				fn_root.id = buf[1];
				fn_root.code = malloc(sizeof(uint16_t) * 4 * 64);
				fn_root.ins_len = 0;
				fn_root.next = NULL;

				first_fn_exists = true;
				fn_i = &fn_root;
			} else {
				fn_i = &fn_root;
				while(fn_i->next != NULL) fn_i = fn_i->next;
				fn_i->next = malloc(sizeof(struct fn_node));
				fn_i = fn_i->next;

				fn_i->id = buf[1];
				fn_i->code = malloc(sizeof(uint16_t) * 4 * 64);
				fn_i->ins_len = 0;
				fn_i->next = NULL;
			}

			ret_reached = false;
			while(!ret_reached) {
				fread(buf, sizeof(uint16_t), 4, fp);
				if(feof(fp)) {
					fatal("file ends before function returns");
				}
				switch(buf[0]) {
				case BC_FN_DEF:
					fatal("function ends without return statement");
					break;
				case BC_RET:
					ret_reached = true;
					/* FALLTHROUGH */
				default:
					memcpy(fn_i->code + (fn_i->ins_len * 4), buf, sizeof(uint16_t) * 4);
					fn_i->ins_len++;
				}
			}

		} else {
			if(feof(fp)) {
				return fn_root;
			} else if(buf[0] == BC_BEGIN_DATA) {
				bool first_str_exists = false;

				while(true) {
					fread(&stridbuf, sizeof(char), 1, fp);
					strbufi = 0;

					do {
						fread(strbuf + strbufi, sizeof(char), 1, fp);
						if(feof(fp)) return fn_root;
						strbufi++;
					} while(strbuf[strbufi-1] != 0);

					printf("read string id: %d contents: %s\n", stridbuf, strbuf);
					if(!first_str_exists) {
						env->str_root.id = stridbuf;
						env->str_root.string = malloc(sizeof(char) * 64);
						strcpy(env->str_root.string, strbuf);
						env->str_root.next = NULL;
					} else {
						struct str_node *current = &(env->str_root);
						while(current->next != NULL) current = current->next;
						current->next = malloc(sizeof(struct str_node));
						current = current->next;

						current->id = stridbuf;
						current->string = malloc(sizeof(char) * 64);
						strcpy(current->string, strbuf);
						current->next = NULL;
					}
				}
			} else {
				fatal("naked bytecode");
			}
		}
	}
}

const char *reg_to_str(uint16_t reg)
{
	switch(reg) {
	case REG_A:
		return "reg_a";
	case REG_B:
		return "reg_b";
	case REG_C:
		return "reg_c";
	case REG_D:
		return "reg_D";
	default:
		fatal("register not found");
		return NULL;
	}
}

uint16_t *get_reg(struct vm_env *env, uint16_t reg)
{
	switch(reg) {
	case REG_A:
		return &(env->reg_a);
	case REG_B:
		return &(env->reg_b);
	case REG_C:
		return &(env->reg_c);
	case REG_D:
		return &(env->reg_d);
	default:
		fatal("register not found");
		return NULL;
	}
}

void dump_bytecode(struct fn_node root)
{
	struct fn_node *current = &root;
	int i;
	uint16_t ins;
	uint16_t args[3];

	while(current != NULL) {
		printf("-- FUNCTION %d --\n", current->id);
		for(i = 0; i < current->ins_len; i++) {
			ins = current->code[i * 4];
			memcpy(args, current->code + i * 4 + 1, sizeof(uint16_t) * 3);
			printf("%2d: ", i);
			switch(ins) {
			case BC_RET:
				printf("ret\n");
				break;
			case BC_LOAD_CONSTANT:
				printf("load_constant: %s %d\n", reg_to_str(args[0]), args[1]);
				break;
			case BC_PRINT_NUM:
				printf("print_num: %s\n", reg_to_str(args[0]));
				break;
			case BC_CALL:
				printf("call: %d\n", args[0]);
				break;
			case BC_LOCAL_JMP:
				printf("local_jmp: %d\n", args[0]);
				break;
			case BC_LOCAL_JE:
				printf("local_je: %d\n", args[0]);
				break;
			case BC_LOCAL_JNE:
				printf("local_jne: %d\n", args[0]);
				break;
			case BC_REL_JMP:
				printf("rel_jmp: %d\n", args[0]);
				break;
			case BC_REL_JE:
				printf("rel_je: %d\n", args[0]);
				break;
			case BC_REL_JNE:
				printf("rel_jne: %d\n", args[0]);
				break;
			case BC_CMP:
				printf("cmp: %s %s\n", reg_to_str(args[0]), reg_to_str(args[1]));
				break;
			case BC_PRINT_STR:
				printf("_print_str: %d\n", args[0]);
				break;
			default:
				printf("(dump unimplemented for %d)\n", ins);
				break;
			}
		}
		current = current->next;
	}
}

void eval_bc_fn(struct fn_node root, struct vm_env *env, uint16_t id)
{
	printf("\nevaluating function %d:\n", id);
	struct fn_node *current = &root;
	int i;
	uint16_t ins = 0, args[3];
	struct str_node *current_str = &(env->str_root);

	while(current->id != id && current->next != NULL) current = current->next;
	if(current->id != id) fatal("function not found!");

	for(i = 0; i < current->ins_len; i++) {
		ins = current->code[i * 4];
		memcpy(args, current->code + i * 4 + 1, sizeof(uint16_t) * 3);

		// evaluation happens here!

		switch(ins) {
		case BC_PRINT_NUM:
			printf("number: %d\n", *get_reg(env, args[0]));
			break;
		case BC_PRINT_STR:
			while(current_str->id != args[0] && current_str->next != NULL) current_str = current_str->next;
			if(current_str->id != args[0]) fatal("string not found");
			printf("%s", current_str->string);
			break;
		case BC_LOAD_CONSTANT:
			*get_reg(env, args[0]) = args[1];
			break;
		case BC_RET:
			return;
			break;
		case BC_CALL:
			eval_bc_fn(root, env, args[0]);
			break;
		case BC_CMP:
			env->cmp_flag = *get_reg(env, args[0]) - *get_reg(env, args[1]);
			break;
		case BC_LOCAL_JMP:
			// because it is incremented at the end of this loop
			i = args[0] - 1;
			break;
		case BC_LOCAL_JE:
			if(env->cmp_flag == 0) i = args[0] - 1;
			break;
		case BC_LOCAL_JNE:
			if(env->cmp_flag != 0) i = args[0] - 1;
			break;
		case BC_REL_JMP:
			i += args[0];
			break;
		case BC_REL_JE:
			if(env->cmp_flag == 0) i += args[0];
			break;
		case BC_REL_JNE:
			if(env->cmp_flag != 0) i += args[0];
			break;
		default:
			printf("eval unimplemented for %d\n", ins);
			break;
		}
	}
}

void free_bytecode(struct fn_node root)
{
	struct fn_node *current = &root;
	while(current != NULL) {
		free(current->code);
		current = current->next;
	}
}

int main(int argc, char *argv[])
{
	printf("vm\n");
	if(argc < 2) {
		printf("usage: %s <vm-file>\n", argv[0]);
	} else {
		FILE *fp;
		fp = fopen(argv[1], "rb");
		if(fp == NULL) {
			printf("can't open file %s\n", argv[1]);
		} else {
			printf("opened file\n");
			struct fn_node fn_root;
			struct vm_env env;
			fn_root = load_bytecode(fp, &env);
			fclose(fp);
			printf("dump:\n\n");
			dump_bytecode(fn_root);
			eval_bc_fn(fn_root, &env, 0);
			free_bytecode(fn_root);
		}
	}
	return 0;
}
