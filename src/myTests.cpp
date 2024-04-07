#include <gtest/gtest.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include "../include/emu4380.h"

TEST(FetchTest, FetchNormalOperation){
	int size = 131072;
	prog_mem = new unsigned char[size];
	reg_file[RegNames::PC] = 10;

	EXPECT_TRUE(fetch());

	delete[] prog_mem;
}

TEST(FetchTest, FetchAtMemoryBoundary){
  int size = 131072;
  prog_mem = new unsigned char[size];
  reg_file[RegNames::PC] = size - 8;

  EXPECT_TRUE(fetch());

  delete[] prog_mem;
}

TEST(FetchTest, FetchBeyondMemoryBoundary){
  int size = 131072;
  prog_mem = new unsigned char[size];
  reg_file[RegNames::PC] = size - 7;

  EXPECT_FALSE(fetch());

  delete[] prog_mem;
}

TEST(FetchTest, FetchWithInvalidPC){
  int size = 131072;
  prog_mem = new unsigned char[size];
  reg_file[RegNames::PC] = size + 10;

  EXPECT_FALSE(fetch());

  delete[] prog_mem;
}


TEST(FetchTest, FetchAtMemoryLimit){
  int size = 131072;
  prog_mem = new unsigned char[size];
  reg_file[RegNames::PC] = 10;

  EXPECT_TRUE(fetch());

  delete[] prog_mem;
}

TEST(FetchTest, FetchOverMemoryLimit){
  int size = 131072;
  prog_mem = new unsigned char[size];
  reg_file[RegNames::PC] = size - 7;

  EXPECT_FALSE(fetch());

  delete[] prog_mem;
}


TEST(DecodeTest, ValidOperation){
	cntrl_regs[CntrlRegNames::OPERATION] = 7; 
  cntrl_regs[CntrlRegNames::OPERAND_1] = 1; 
  cntrl_regs[CntrlRegNames::OPERAND_2] = 2;

	EXPECT_TRUE(decode());
}

TEST(DecodeTest, InvalidOperation){
  cntrl_regs[CntrlRegNames::OPERATION] = 99;

  EXPECT_FALSE(decode());
}

TEST(DecodeTest, InvalidOperand){
	cntrl_regs[CntrlRegNames::OPERATION] = 7;
  cntrl_regs[CntrlRegNames::OPERAND_1] = 99;
  cntrl_regs[CntrlRegNames::OPERAND_2] = 2;

  EXPECT_FALSE(decode());
}

TEST(DecodeTest, OperandEdgeValues){
  cntrl_regs[CntrlRegNames::OPERATION] = 7;
  cntrl_regs[CntrlRegNames::OPERAND_1] = 0;
  cntrl_regs[CntrlRegNames::OPERAND_2] = num_gen_regs - 1;

  EXPECT_TRUE(decode());
}

TEST(DecodeTest, DecodeWithoutFetching){
  cntrl_regs[CntrlRegNames::OPERATION] = 7;
  cntrl_regs[CntrlRegNames::OPERAND_2] = 1;

	decode();
	EXPECT_TRUE(decode());
}

TEST(DecodeTest, UninitialzedCntrlRegisters){
	for(int i = 0; i < num_cntrl_regs; i++){
		cntrl_regs[i] = 0xFFFFFFFF;
	}

  EXPECT_FALSE(decode());
}

TEST(DecodeTest, DecodeMOVValid) {
    cntrl_regs[OPERATION] = 7; // MOV operation code
    cntrl_regs[OPERAND_1] = 2; // Destination register (R2)
    cntrl_regs[OPERAND_2] = 3; // Source register (R3)
    cntrl_regs[OPERAND_3] = 0xFFFFFFFF; // Not used for MOV, set to an example invalid value
    reg_file[R3] = 3;

    bool success = decode();

    ASSERT_TRUE(success);

    EXPECT_EQ(data_regs[REG_VAL_1], reg_file[R3]);
}

TEST(DecodeTest, DecodeMOVIValid) {
    cntrl_regs[OPERATION] = 8; // MOVI operation code
    cntrl_regs[OPERAND_1] = 1; // Destination register (R1)
    cntrl_regs[IMMEDIATE] = 1234;

    bool success = decode();

    ASSERT_TRUE(success);
}

TEST(DecodeTest, DecodeLDAValid) {
    cntrl_regs[OPERATION] = 9; // LDA operation code
    cntrl_regs[OPERAND_1] = 2; // Destination register (R2)
    cntrl_regs[IMMEDIATE] = 0x00400000;

    bool success = decode();

    ASSERT_TRUE(success);
}

TEST(DecodeTest, DecodeLDRValid) {
    cntrl_regs[OPERATION] = 11; // LDR operation code
    cntrl_regs[OPERAND_1] = 3; // Destination register (R3)
    cntrl_regs[IMMEDIATE] = 0x00400004;

    bool success = decode();

    ASSERT_TRUE(success);
}

TEST(DecodeTest, DecodeLDBValid) {
    cntrl_regs[OPERATION] = 13; // LDB operation code
    cntrl_regs[OPERAND_1] = 4; // Destination register (R4)
    cntrl_regs[IMMEDIATE] = 0x00400008;

    bool success = decode();

    ASSERT_TRUE(success);
}

TEST(DecodeTest, DecodeSTRValid) {
    cntrl_regs[OPERATION] = 10; // STR operation code
    cntrl_regs[OPERAND_1] = 5; // Source register (R5)
    cntrl_regs[IMMEDIATE] = 0x00400010;

    reg_file[5] = 2021;

    bool success = decode();

    ASSERT_TRUE(success);
    EXPECT_EQ(data_regs[REG_VAL_1], reg_file[5]);
}

TEST(DecodeTest, DecodeSTBValid) {
    cntrl_regs[OPERATION] = 12; // STB operation code
    cntrl_regs[OPERAND_1] = 6; // Source register (R6)
    cntrl_regs[IMMEDIATE] = 0x00400014;

    reg_file[6] = 0xAABBCCDD;

    bool success = decode();

    ASSERT_TRUE(success);
    EXPECT_EQ(data_regs[REG_VAL_1], reg_file[6]);
}

TEST(DecodeTest, DecodeADDValid) {
    cntrl_regs[OPERATION] = 18; // ADD operation code
    cntrl_regs[OPERAND_1] = 4; // Destination register (R4)
    cntrl_regs[OPERAND_2] = 2; // First source register (R2)
    cntrl_regs[OPERAND_3] = 3; // Second source register (R3)

    reg_file[2] = 10; // Value in R2
    reg_file[3] = 20; // Value in R3

    bool success = decode();

    ASSERT_TRUE(success);
    EXPECT_EQ(data_regs[REG_VAL_1], reg_file[2]);
    EXPECT_EQ(data_regs[REG_VAL_2], reg_file[3]);
}

TEST(DecodeTest, DecodeSUBValid) {
    cntrl_regs[OPERATION] = 20; // SUB operation code
    cntrl_regs[OPERAND_1] = 4; // Destination register (R4)
    cntrl_regs[OPERAND_2] = 5; // First source register (R5)
    cntrl_regs[OPERAND_3] = 6; // Second source register (R6)

    reg_file[5] = 30; // Value in R5
    reg_file[6] = 10; // Value in R6

    bool success = decode();

    ASSERT_TRUE(success);
    EXPECT_EQ(data_regs[REG_VAL_1], reg_file[5]);
    EXPECT_EQ(data_regs[REG_VAL_2], reg_file[6]);
}

TEST(DecodeTest, DecodeMULValid) {
    cntrl_regs[OPERATION] = 22; // MUL operation code
    cntrl_regs[OPERAND_1] = 7; // Destination register (R7)
    cntrl_regs[OPERAND_2] = 8; // First source register (R8)
    cntrl_regs[OPERAND_3] = 9; // Second source register (R9)

    reg_file[8] = 4; // Value in R8
    reg_file[9] = 5; // Value in R9

    bool success = decode();

    ASSERT_TRUE(success);
    EXPECT_EQ(data_regs[REG_VAL_1], reg_file[8]);
    EXPECT_EQ(data_regs[REG_VAL_2], reg_file[9]);
}

TEST(DecodeTest, DecodeDIVValid) {
    cntrl_regs[OPERATION] = 24; // DIV operation code
    cntrl_regs[OPERAND_1] = 10; // Destination register (R10)
    cntrl_regs[OPERAND_2] = 11; // First source register (R11)
    cntrl_regs[OPERAND_3] = 12; // Second source register (R12)

    reg_file[11] = 50; // Value in R11
    reg_file[12] = 5;  // Value in R12

    bool success = decode();

    ASSERT_TRUE(success);
    EXPECT_EQ(data_regs[REG_VAL_1], reg_file[11]);
    EXPECT_EQ(data_regs[REG_VAL_2], reg_file[12]);
}

TEST(DecodeTest, DecodeSDIVValid) {
    cntrl_regs[OPERATION] = 25; // SDIV operation code
    cntrl_regs[OPERAND_1] = 13; // Destination register (R13)
    cntrl_regs[OPERAND_2] = 14; // First source register (R14)
    cntrl_regs[OPERAND_3] = 15; // Second source register (R15)

    reg_file[14] = -100; // Value in R14
    reg_file[15] = 4;    // Value in R15

    bool success = decode();

    ASSERT_TRUE(success);
    EXPECT_EQ(data_regs[REG_VAL_1], reg_file[14]);
    EXPECT_EQ(data_regs[REG_VAL_2], reg_file[15]);
}

TEST(DecodeTest, DecodeADDIValid) {
    cntrl_regs[OPERATION] = 19; // ADDI operation code
    cntrl_regs[OPERAND_1] = 1; // Destination register (R1)
    cntrl_regs[OPERAND_2] = 2; // Source register (R2)
    cntrl_regs[IMMEDIATE] = 5;

    reg_file[2] = 20;

    bool success = decode();

    ASSERT_TRUE(success);
    EXPECT_EQ(data_regs[REG_VAL_1], reg_file[2]);
}

TEST(DecodeTest, DecodeSUBIValid) {
    cntrl_regs[OPERATION] = 21; // SUBI operation code
    cntrl_regs[OPERAND_1] = 3; // Destination register (R3)
    cntrl_regs[OPERAND_2] = 4; // Source register (R4)
    cntrl_regs[IMMEDIATE] = 10; // Immediate value to subtract

    reg_file[4] = 30;

    bool success = decode();

    ASSERT_TRUE(success);
    EXPECT_EQ(data_regs[REG_VAL_1], reg_file[4]);
}

TEST(DecodeTest, DecodeMULIValid) {
    cntrl_regs[OPERATION] = 23; // MULI operation code
    cntrl_regs[OPERAND_1] = 5; // Destination register (R5)
    cntrl_regs[OPERAND_2] = 6; // Source register (R6)
    cntrl_regs[IMMEDIATE] = 2;

    reg_file[6] = 15;

    bool success = decode();

    ASSERT_TRUE(success);
    EXPECT_EQ(data_regs[REG_VAL_1], reg_file[6]);
}

TEST(DecodeTest, DecodeDIVIValid) {
    cntrl_regs[OPERATION] = 26; // DIVI operation code
    cntrl_regs[OPERAND_1] = 7; // Destination register (R7)
    cntrl_regs[OPERAND_2] = 8; // Source register (R8)
    cntrl_regs[IMMEDIATE] = 4;

    reg_file[8] = 40;

    bool success = decode();

    ASSERT_TRUE(success);
    EXPECT_EQ(data_regs[REG_VAL_1], reg_file[8]);
}

TEST(ExecuteTest, JMPValid) {
    cntrl_regs[OPERATION] = 1; // JMP operation code
    cntrl_regs[IMMEDIATE] = 100;

    execute();

    EXPECT_EQ(reg_file[PC], 100);
}

TEST(ExecuteTest, MOVIValid) {
    cntrl_regs[OPERATION] = 8; // MOVI operation code
    cntrl_regs[OPERAND_1] = 5; // Destination register (R5)
    cntrl_regs[IMMEDIATE] = 42;

    execute();

    EXPECT_EQ(reg_file[5], 42);
}

TEST(ExecuteDecodeTest, MOVValid) {
    cntrl_regs[OPERATION] = 7; // MOV operation code
    cntrl_regs[OPERAND_1] = 4; // Destination register (R4)
    cntrl_regs[OPERAND_2] = 5; // Source register (R5)
    reg_file[5] = 99;

    ASSERT_TRUE(decode());

    execute();

    EXPECT_EQ(reg_file[4], 99);
}

TEST(ExecuteDecodeTest, LDAValid) {
    cntrl_regs[OPERATION] = 9; // LDA operation code
    cntrl_regs[OPERAND_1] = 6; // Destination register (R6)
    cntrl_regs[IMMEDIATE] = 12345;

    ASSERT_TRUE(decode());

    execute();

    EXPECT_EQ(reg_file[6], 12345);
}

TEST(ExecuteDecodeTest, STRValid) {
    cntrl_regs[OPERATION] = 10; // STR operation code
    cntrl_regs[OPERAND_1] = 3; // Source register (R3)
    cntrl_regs[IMMEDIATE] = 100;
    reg_file[3] = 2021;

    init_mem(131072);

    execute();

    unsigned int storedValue = *reinterpret_cast<unsigned int*>(prog_mem + 100);
    EXPECT_EQ(storedValue, 2021);
}

TEST(ExecuteDecodeTest, LDRValid) {
    cntrl_regs[OPERATION] = 11; // LDR operation code
    cntrl_regs[OPERAND_1] = 4; // Destination register (R4)
    cntrl_regs[IMMEDIATE] = 200;

    init_mem(131072);
	
    unsigned int expectedValue = 1984;
    *reinterpret_cast<unsigned int*>(prog_mem + 200) = expectedValue;

    execute();

    EXPECT_EQ(reg_file[4], expectedValue);
}

TEST(ExecuteDecodeTest, STBValid) {
    cntrl_regs[OPERATION] = 12; // STB operation code
    cntrl_regs[OPERAND_1] = 3; // Source register (R3)
    cntrl_regs[IMMEDIATE] = 150;
    reg_file[3] = 0xABCD;

    init_mem(131072);

    ASSERT_TRUE(decode());

    execute();

    unsigned char storedByte = prog_mem[150];
    EXPECT_EQ(storedByte, 0xCD);
}

TEST(ExecuteDecodeTest, LDBValid) {
    cntrl_regs[OPERATION] = 13; // LDB operation code
    cntrl_regs[OPERAND_1] = 4; // Destination register (R4)
    cntrl_regs[IMMEDIATE] = 250;

    init_mem(131072);
    prog_mem[250] = 0x7E;

    ASSERT_TRUE(decode());

    execute();

    EXPECT_EQ(reg_file[4], 0x7E);
}

TEST(ExecuteDecodeTest, ADDValid) {
    cntrl_regs[OPERATION] = 18; // ADD operation code
    cntrl_regs[OPERAND_1] = 5; // Destination register (R5)
    cntrl_regs[OPERAND_2] = 2; // Source register 1 (R2)
    cntrl_regs[OPERAND_3] = 3; // Source register 2 (R3)

    reg_file[2] = 10; // Value in R2
    reg_file[3] = 20; // Value in R3

    ASSERT_TRUE(decode());

    execute();

    EXPECT_EQ(reg_file[5], 30);
}

TEST(ExecuteDecodeTest, ADDIValid) {
    cntrl_regs[OPERATION] = 19; // ADDI operation code
    cntrl_regs[OPERAND_1] = 4; // Destination register (R4)
    cntrl_regs[OPERAND_2] = 2; // Source register (R2)
    cntrl_regs[IMMEDIATE] = 15;

    reg_file[2] = 25; // Value in R2

    ASSERT_TRUE(decode());

    execute();

    EXPECT_EQ(reg_file[4], 40);
}

TEST(ExecuteDecodeTest, SUBValid) {
    cntrl_regs[OPERATION] = 20; // SUB operation code
    cntrl_regs[OPERAND_1] = 6; // Destination register (R6)
    cntrl_regs[OPERAND_2] = 3; // Source register 1 (R3)
    cntrl_regs[OPERAND_3] = 2; // Source register 2 (R2)

    reg_file[3] = 50; // Value in R3
    reg_file[2] = 20; // Value in R2

    ASSERT_TRUE(decode());

    execute();

    EXPECT_EQ(reg_file[6], 30);
}

TEST(ExecuteDecodeTest, SUBIValid) {
    cntrl_regs[OPERATION] = 21; // SUBI operation code
    cntrl_regs[OPERAND_1] = 7; // Destination register (R7)
    cntrl_regs[OPERAND_2] = 3; // Source register (R3)
    cntrl_regs[IMMEDIATE] = 5;

    reg_file[3] = 25; // Value in R3

    ASSERT_TRUE(decode());

    execute();

    EXPECT_EQ(reg_file[7], 20);
}

TEST(ExecuteDecodeTest, MULValid) {
    cntrl_regs[OPERATION] = 22; // MUL operation code
    cntrl_regs[OPERAND_1] = 4; // Destination register (R4)
    cntrl_regs[OPERAND_2] = 1; // Source register 1 (R1)
    cntrl_regs[OPERAND_3] = 2; // Source register 2 (R2)

    reg_file[1] = 5; // Value in R1
    reg_file[2] = 4; // Value in R2

    ASSERT_TRUE(decode());

    execute();

    EXPECT_EQ(reg_file[4], 20);
}

TEST(ExecuteDecodeTest, MULIValid) {
    cntrl_regs[OPERATION] = 23; // MULI operation code
    cntrl_regs[OPERAND_1] = 5; // Destination register (R5)
    cntrl_regs[OPERAND_2] = 1; // Source register (R1)
    cntrl_regs[IMMEDIATE] = 3;

    reg_file[1] = 7; // Value in R1

    ASSERT_TRUE(decode());

    execute();

    EXPECT_EQ(reg_file[5], 21);
}

TEST(ExecuteDecodeTest, DIVValid) {
    cntrl_regs[OPERATION] = 24; // DIV operation code
    cntrl_regs[OPERAND_1] = 4; // Destination register (R4)
    cntrl_regs[OPERAND_2] = 1; // Source register 1 (R1)
    cntrl_regs[OPERAND_3] = 2; // Source register 2 (R2)

    reg_file[1] = 20; // Value in R1
    reg_file[2] = 5; // Value in R2

    ASSERT_TRUE(decode());

    execute();

    EXPECT_EQ(reg_file[4], 4);
}

TEST(ExecuteDecodeTest, SDIVValid) {
    cntrl_regs[OPERATION] = 25; // SDIV operation code
    cntrl_regs[OPERAND_1] = 5; // Destination register (R5)
    cntrl_regs[OPERAND_2] = 3; // Source register 1 (R3)
    cntrl_regs[OPERAND_3] = 4; // Source register 2 (R4)

    reg_file[3] = -20;
    reg_file[4] = 5;

    ASSERT_TRUE(decode());

    execute();

    EXPECT_EQ(reg_file[5], static_cast<unsigned int>(-4));
}

TEST(ExecuteDecodeTest, DIVIValid) {
    cntrl_regs[OPERATION] = 26; // DIVI operation code
    cntrl_regs[OPERAND_1] = 6; // Destination register (R6)
    cntrl_regs[OPERAND_2] = 3; // Source register (R3)
    cntrl_regs[IMMEDIATE] = 4;

    reg_file[3] = 16; // Value in R3

    ASSERT_TRUE(decode());

    execute();

    EXPECT_EQ(reg_file[6], 4);
}

/*
TEST(ExecuteTest, AddOperation){
  cntrl_regs[OPERATION] = 18;
  cntrl_regs[OPERAND_1] = 0;
  cntrl_regs[OPERAND_2] = 1;
  cntrl_regs[OPERAND_3] = 2;
  reg_file[1] = 5;
  reg_file[2] = 10;

  EXPECT_TRUE(execute());
  EXPECT_EQ(reg_file[0], 15);
}

TEST(ExecuteTest, SubOperation){
  cntrl_regs[OPERATION] = 20;
  cntrl_regs[OPERAND_1] = 0;
  cntrl_regs[OPERAND_2] = 1;
  cntrl_regs[OPERAND_3] = 2;
  reg_file[1] = 10;
  reg_file[2] = 5;

  EXPECT_TRUE(execute());
  EXPECT_EQ(reg_file[0], 5);
}

TEST(ExecuteTest, MULOperation){
  cntrl_regs[OPERATION] = 22;
  cntrl_regs[OPERAND_1] = 0;
  cntrl_regs[OPERAND_2] = 1;
  cntrl_regs[OPERAND_3] = 2;
  reg_file[1] = 5;
  reg_file[2] = 10;

  EXPECT_TRUE(execute());
  EXPECT_EQ(reg_file[0], 50);
}

TEST(ExecuteTest, DivOperation){
  cntrl_regs[OPERATION] = 24;
  cntrl_regs[OPERAND_1] = 0;
  cntrl_regs[OPERAND_2] = 1;
  cntrl_regs[OPERAND_3] = 2;
  reg_file[1] = 10;
  reg_file[2] = 2;

  EXPECT_TRUE(execute());
  EXPECT_EQ(reg_file[0], 5);
}

TEST(ExecuteTest, StoreMemoryLimit){
	int size = 131072;
	cntrl_regs[OPERATION] = 10;
	cntrl_regs[OPERAND_1] = 1;
	cntrl_regs[IMMEDIATE] = size + 1;
	reg_file[1] = 12345;

	EXPECT_FALSE(execute());
}

TEST(ExecuteTest, LoadOperation){
  cntrl_regs[OPERATION] = 11;
  cntrl_regs[OPERAND_1] = 1;
  cntrl_regs[IMMEDIATE] = 100;
  auto* address = reinterpret_cast<unsigned int*>(prog_mem + 100);
  *address = 54321;

  EXPECT_TRUE(execute());
  EXPECT_EQ(reg_file[1], 54321);
}

TEST(ExecuteTest, DivByZero){
  cntrl_regs[OPERATION] = 24;
  cntrl_regs[OPERAND_2] = 2;
  cntrl_regs[OPERAND_3] = 3;

  reg_file[2] = 50;
  reg_file[3] = 0;

  EXPECT_FALSE(execute());
}
*/

TEST(MemoryInitializationTest, AllocatesMem){
        unsigned int size = 131072;
        bool success = init_mem(size);

        ASSERT_TRUE(success);

        //verify mem is zeroed out
        for (unsigned int i = 0; i < size; ++i) {
        EXPECT_EQ(0, prog_mem[i]) << "Memory at index " << i << " is not zero-initialized";
  }
}

TEST(FetchTest, PCIncrementsProperly){
        unsigned int size = 131072;
  init_mem(size);

         reg_file[RegNames::PC] = 0;

        prog_mem[0] = 0x01;
  prog_mem[1] = 0x02;
  prog_mem[2] = 0x03;
  prog_mem[3] = 0x04;
  prog_mem[4] = 0x08;
  prog_mem[5] = 0x07;
  prog_mem[6] = 0x06;
  prog_mem[7] = 0x05;

        bool success = fetch();

        // Check that PC incremented by 8
  EXPECT_EQ(reg_file[RegNames::PC], 8u) << "PC did not increment by 8 after fetch.";
}

TEST(FetchTest, ValidatesControlRegistersInitialization) {
    unsigned int size = 131072;
        init_mem(size);

    unsigned char instruction[8] = {
        0x01, 0x02, 0x03, 0x04, 0x08, 0x07, 0x06, 0x05
    };
                std::memcpy(prog_mem, instruction, sizeof(instruction));

    reg_file[PC] = 0;

    bool fetchSuccess = fetch();

    ASSERT_TRUE(fetchSuccess);

    EXPECT_EQ(cntrl_regs[OPERATION], 0x01);
    EXPECT_EQ(cntrl_regs[OPERAND_1], 0x02);
    EXPECT_EQ(cntrl_regs[OPERAND_2], 0x03);
    EXPECT_EQ(cntrl_regs[OPERAND_3], 0x04);
    EXPECT_EQ(cntrl_regs[IMMEDIATE], 0x05060708);
}

TEST(RegisterTest, HandlesValidRegisters){
        unsigned int validReg = 5;
        EXPECT_TRUE(isValidRegister(validReg));
}

TEST(RegisterTest, HandlesInvalidRegisters){
  unsigned int validReg = 23;
  EXPECT_FALSE(isValidRegister(validReg));
}


TEST(TrapTests, TRPWriteIntToStdOut){
        cntrl_regs[OPERATION] = 31;
        cntrl_regs[IMMEDIATE] = 1;
        reg_file[R3] = 123;

  std::stringstream buffer;
  std::streambuf* prevCoutStreamBuf = std::cout.rdbuf();
  std::cout.rdbuf(buffer.rdbuf());

  execute();

  std::cout.rdbuf(prevCoutStreamBuf);

  EXPECT_EQ(buffer.str(), "123");
}

int main(int argc, char **argv){
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
};
