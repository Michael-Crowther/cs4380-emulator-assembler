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
