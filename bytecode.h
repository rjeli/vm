#include <stdint.h>

// OP CODES
// don't use 0x00 - it may be confused 
// with uninitialized values
const uint16_t BC_FN_DEF				= 0x01;
const uint16_t BC_RET						= 0x02;
const uint16_t BC_PRINT_NUM			= 0x03;
const uint16_t BC_CALL					= 0x04;
const uint16_t BC_LOCAL_JMP			= 0x05;
const uint16_t BC_LOCAL_JE			= 0x06;
const uint16_t BC_LOCAL_JNE			= 0x07;
const uint16_t BC_CMP						= 0x08;
const uint16_t BC_REL_JMP				= 0x09;
const uint16_t BC_REL_JE				= 0x0A;
const uint16_t BC_REL_JNE				= 0x0B;
const uint16_t BC_LOAD_CONSTANT = 0x0C;
const uint16_t BC_BEGIN_DATA		= 0x0D;
const uint16_t BC_PRINT_STR			= 0x0E;

// REGISTER CODES
const uint16_t REG_A						= 0x00;
const uint16_t REG_B						= 0x01;
const uint16_t REG_C						= 0x02;
const uint16_t REG_D						= 0x03;
