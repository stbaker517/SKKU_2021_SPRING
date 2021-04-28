#include<stdio.h>
#include<stdlib.h>

typedef unsigned int Data;
unsigned int Instruction;
unsigned int PC;
unsigned int Reg[32] = { 0, };
unsigned int Inst_Memory[64001];
unsigned int Data_Memory[64001];

Data Inst_31to26;
Data Inst_25to21;
Data Inst_20to16;
Data Inst_15to11;
Data Inst_15to00;
Data Inst_05to00;
Data Inst_25to00;

Data isZero;
Data ALUResult;

unsigned int RegDst;
unsigned int ALUSrc;
unsigned int MemtoReg;
unsigned int RegWrite;
unsigned int MemRead;
unsigned int MemWrite;
unsigned int Branch;
unsigned int ALUOP1;
unsigned int ALUOP0;
Data ReadData1;
Data ReadData2;
/*
Arithmetic/logical: add, sub, and, or, slt ,

Arithmetic/logical with immediate: addi , andi , ori , lui , slti

Memory access: lw , sw , lh lhu , sh , lb , lbu , sb

Control transfer: beq , bne , j

Shift instructions: sll , srl
*/

void ControlUnit(unsigned int OPcode);
void Registers(Data RegWrite, Data ReadRegister1, Data ReadRegister2, Data WriteRegister, Data WriteData);
void fetchInstruction(Data Inst);
void ALU(Data Se_Inst15to00);

int main() {
	
	//Initializing
	for (int i = 0;i < 64001;i++)
	{
		Inst_Memory[i] = 0xFF;
		Data_Memory[i] = 0xFF;
	}





}

/*
Arithmetic/logical: add, sub, and, or, slt ,

Arithmetic/logical with immediate: addi , andi , ori , lui , slti

Memory access: lw , sw , lh lhu , sh , lb , lbu , sb

Control transfer: beq , bne , j

Shift instructions: sll , srl
*/

void ControlUnit(unsigned int OPcode)
{
	unsigned int op0, op1, op2, op3, op4, op5,comb0,comb1,comb2,comb3;
	op5 = (OPcode & 0b100000) >> 5;
	op4 = (OPcode & 0b010000) >> 4;
	op3 = (OPcode & 0b001000) >> 3;
	op2 = (OPcode & 0b000100) >> 2;
	op1 = (OPcode & 0b000010) >> 1;
	op0 = (OPcode & 0b000001) >> 0;

	comb0 = !(op0 | op1 | op2 | op3 | op4 | op5);
	comb1 = (op5 & !op4 & !op3 & !op2 & op1 & op0);
	comb2 = (op5 & !op4 & op3 & !op2 & op1 & op0);
	comb3=(!op5 & !op4 & !op3 & op2 & !op1 & !op0);

	RegDst = comb0;
	ALUSrc = comb1 | comb2;
	MemtoReg = comb1;
	RegWrite = comb0 | comb1;
	MemRead = comb1;
	MemWrite = comb2;
	Branch = comb3;
	ALUOP1 = comb1;
	ALUOP0 = comb3;

}

void Registers(Data RegWrite, Data ReadRegister1, Data ReadRegister2, Data WriteRegister, Data WriteData)
{
	ReadData1 = Data_Memory[ReadRegister1];
	ReadData2 = Data_Memory[ReadRegister2];

	if (RegWrite)
	{
		Data_Memory[WriteRegister] = WriteData;
	}
}

//0000 0000 0000 0000 0000 0000 0000 0000 
void fetchInstruction(Data Inst)
{
	Inst_31to26 = (Inst & 0b11111100000000000000000000000000) >> 26;
	Inst_25to21 = (Inst & 0b00000011111000000000000000000000) >> 21;
	Inst_20to16=(Inst & 0b00000000000111110000000000000000) >> 16;
	Inst_15to11 = (Inst & 0b00000000000000001111100000000000) >> 11;
	Inst_15to00 = Inst & 0b000000000000000011111111111111111;
	Inst_05to00 = Inst & 0b111111;
	Inst_25to00 = Inst & 0b11111111111111111111111111;


}

void ALU(Data Se_Inst15to00)
{
	Data input1,input2;
	input1=ReadData1;
	input2=ALUSrc?Se_Inst15to00:ReadData2;

	//다양한 작업
	//
	
}