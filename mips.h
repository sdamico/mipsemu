#include <stdio.h>
#include <malloc.h>

// Processor features
#define MIPS_BIG_ENDIAN

// Opcodes
#define	OPCODE_R	0b000000
#define OPCODE_J	0b000010
#define OPCODE_JAL	0b000011
#define OPCODE_ADDI	0b001000
#define OPCODE_ADDIU	0b001001
#define OPCODE_ANDI	0b001100
#define OPCODE_BEQ	0b000100
#define OPCODE_BGEZ_BLTZ	0b000001
#define OPCODE_BGTZ	0b000111
#define OPCODE_BLEZ	0b000110
#define OPCODE_BNE	0b000101
#define OPCODE_LB	0b100000
#define OPCODE_LBU	0b100100
#define OPCODE_LH	0b100001
#define OPCODE_LHU	0b100101
#define OPCODE_LUI	0b001111
#define OPCODE_LW	0b100011
#define OPCODE_LWCL	0b110001
#define OPCODE_ORI	0b001101
#define OPCODE_SB	0b101000
#define OPCODE_SLTI	0b001010
#define OPCODE_SLTIU	0b001011
#define OPCODE_SH	0b101001
#define OPCODE_SW	0b101011
#define OPCODE_SWCL	0b111001
#define OPCODE_XORI	0b001110

// Function Codes
#define FCODE_ADD	0b100000
#define FCODE_ADDU	0b100001
#define FCODE_AND	0b100100
#define FCODE_BREAK	0b001101
#define FCODE_DIV	0b011010
#define FCODE_DIVU	0b011011
#define FCODE_JALR	0b001001
#define FCODE_JR	0b001000
#define FCODE_MFHI	0b010000
#define FCODE_MFLO	0b010010
#define FCODE_MTHI	0b010001
#define FCODE_MTLO	0b010011
#define FCODE_MULT	0b011000
#define FCODE_MULTU	0b011001
#define FCODE_NOR	0b100111
#define FCODE_OR	0b100101
#define FCODE_SLL	0b000000
#define FCODE_SLLV	0b000100
#define FCODE_SLT	0b101010
#define FCODE_SLTU	0b101011
#define FCODE_SRA	0b000011
#define FCODE_SRAV	0b000111
#define FCODE_SRL	0b000010
#define FCODE_SRLV	0b000110
#define FCODE_SUB	0b100010
#define FCODE_SUBU	0b100011
#define FCODE_SYSCALL	0b001100
#define FCODE_XOR	0b100110

// Struct that defines our MIPS cpu
typedef struct {
	unsigned int registers[32];	// general purpose registers
	unsigned int lo, hi;		// mult/div special registers
	unsigned int pc;		// program counter
	void *memory;			// pointer to memory
	size_t memorySize;		// size of memory (bytes)
	unsigned int codeOffset;	// offset to program
	unsigned int ptrGFX;		// address of graphics memory
	unsigned char mmapGFX;		// 0->1 when we write to graphics memory with an instruction
					// This signals the renderer to grab the pixel from graphics memory
	unsigned int ptrMouse;		// address of mouse memory mapping
	unsigned int ptrArrowKeys;	// address of arrow key mapping
	int mouseX, mouseY;		// mouse coordinates
	int arrowKeys;			// Arrow Key Data
} MIPS;

unsigned int byteSwap(unsigned int word);

// Initialize the processor - will allocate memory of size memorySize, initialize the PC, and 
MIPS *initMIPS(size_t memorySize, unsigned int pc, unsigned int ptrGFX, unsigned int ptrArrowKeys, unsigned int ptrMouse);

// Free our CPU
void freeMIPS(MIPS *m);

// Load a program to address offset "offset"
int loadProgram(MIPS *m, char *file, int offset);

// Emulate a _single_ cycle of the CPU
unsigned int emulate(MIPS *m);
