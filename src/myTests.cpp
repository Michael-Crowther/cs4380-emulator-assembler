#include <gtest/gtest.h>
#include "../include/emu4380.h"

TEST(FetchTest, FetchNormalOperation){
	memorySize = 1000;
	prog_mem = new unsigned char[memorySize];
	reg_file[static_cast<int>(RegNames::PC)] = 10;

	EXPECT_TRUE(fetch());

	delete[] prog_mem;
}

TEST(FetchTest, FetchAtMemoryBoundary){
  memorySize = 1000;
  prog_mem = new unsigned char[memorySize];
  reg_file[static_cast<int>(RegNames::PC)] = memorySize - 8;

  EXPECT_TRUE(fetch());

  delete[] prog_mem;
}

TEST(FetchTest, FetchBeyondMemoryBoundary){
  memorySize = 1000;
  prog_mem = new unsigned char[memorySize];
  reg_file[static_cast<int>(RegNames::PC)] = memorySize - 7;

  EXPECT_FALSE(fetch());

  delete[] prog_mem;
}

TEST(FetchTest, FetchWithInvalidPC){
  memorySize = 1000;
  prog_mem = new unsigned char[memorySize];
  reg_file[static_cast<int>(RegNames::PC)] = memorySize + 10;

  EXPECT_FALSE(fetch());

  delete[] prog_mem;
}

TEST(FetchTest, FetchWithZeroMemorySize){
  memorySize = 0;
  prog_mem = new unsigned char[memorySize];
  reg_file[static_cast<int>(RegNames::PC)] = 0;

  EXPECT_FALSE(fetch());

  delete[] prog_mem;
}


TEST(FetchTest, FetchAtMemoryLimit){
  memorySize = 131072;
  prog_mem = new unsigned char[memorySize];
  reg_file[static_cast<int>(RegNames::PC)] = 10;

  EXPECT_TRUE(fetch());

  delete[] prog_mem;
}

TEST(FetchTest, FetchOverMemoryLimit){
  memorySize = 131073;
  prog_mem = new unsigned char[memorySize];
  reg_file[static_cast<int>(RegNames::PC)] = memorySize - 7;

  EXPECT_FALSE(fetch());

  delete[] prog_mem;
}

TEST(DecodeTest, ValidOperation){
	cntrl_regs[static_cast<int>(CntrlRegNames::OPERATION)] = 7; 
  cntrl_regs[static_cast<int>(CntrlRegNames::OPERAND_1)] = 1; 
  cntrl_regs[static_cast<int>(CntrlRegNames::OPERAND_2)] = 2;

	EXPECT_TRUE(decode());
}

TEST(DecodeTest, InvalidOperation){
  cntrl_regs[static_cast<int>(CntrlRegNames::OPERATION)] = 99;

  EXPECT_FALSE(decode());
}

TEST(DecodeTest, InvalidOperand){
	cntrl_regs[static_cast<int>(CntrlRegNames::OPERATION)] = 7;
  cntrl_regs[static_cast<int>(CntrlRegNames::OPERAND_1)] = 99;
  cntrl_regs[static_cast<int>(CntrlRegNames::OPERAND_2)] = 2;

  EXPECT_FALSE(decode());
}

TEST(DecodeTest, ImmediateAssignment){
	cntrl_regs[static_cast<int>(CntrlRegNames::OPERATION)] = 8;
  cntrl_regs[static_cast<int>(CntrlRegNames::OPERAND_1)] = 1;
  cntrl_regs[static_cast<int>(CntrlRegNames::IMMEDIATE)] = 12345;
  
	EXPECT_TRUE(decode());
	EXPECT_EQ(data_regs[1], 12345);
}

TEST(DecodeTest, OperandEdgeValues){
  cntrl_regs[static_cast<int>(CntrlRegNames::OPERATION)] = 7;
  cntrl_regs[static_cast<int>(CntrlRegNames::OPERAND_1)] = 0;
  cntrl_regs[static_cast<int>(CntrlRegNames::OPERAND_2)] = num_gen_regs - 1;

  EXPECT_TRUE(decode());
}

TEST(DecodeTest, DecodeWithoutFetching){
  cntrl_regs[static_cast<int>(CntrlRegNames::OPERATION)] = 7;
  cntrl_regs[static_cast<int>(CntrlRegNames::OPERAND_2)] = 1;

	decode();
	EXPECT_TRUE(decode());
}

TEST(DecodeTest, UninitialzedCntrlRegisters){
	for(int i = 0; i < num_cntrl_regs; i++){
		cntrl_regs[i] = 0xFFFFFFFF;
	}

  EXPECT_FALSE(decode());
}

TEST(ExecuteTest, AddOperation){
	cntrl_regs[static_cast<int>(CntrlRegNames::OPERATION)] = 18;
	data_regs[0] = 1;
	data_regs[1] = 2;
	data_regs[2] = 3;
	reg_file[2] = 5;
	reg_file[3] = 10;

	EXPECT_TRUE(execute());
	EXPECT_EQ(reg_file[1], 15);
}

TEST(ExecuteTest, SubOperation){
  cntrl_regs[static_cast<int>(CntrlRegNames::OPERATION)] = 20;
  data_regs[0] = 1;
  data_regs[1] = 2;
  data_regs[2] = 3;
  reg_file[2] = 10;
  reg_file[3] = 5;

  EXPECT_TRUE(execute());
  EXPECT_EQ(reg_file[1], 5);
}

TEST(ExecuteTest, MulOperation){
  cntrl_regs[static_cast<int>(CntrlRegNames::OPERATION)] = 22;
  data_regs[0] = 1;
  data_regs[1] = 2;
  data_regs[2] = 3;
  reg_file[2] = 10;
  reg_file[3] = 5;

  EXPECT_TRUE(execute());
  EXPECT_EQ(reg_file[1], 50);
}

TEST(ExecuteTest, DivOperation){
  cntrl_regs[static_cast<int>(CntrlRegNames::OPERATION)] = 24;
  data_regs[0] = 1;
  data_regs[1] = 2;
  data_regs[2] = 3;
  reg_file[2] = 10;
  reg_file[3] = 5;

  EXPECT_TRUE(execute());
  EXPECT_EQ(reg_file[1], 2);
}

TEST(ExecuteTest, StoreMemoryLimit){
	cntrl_regs[static_cast<int>(CntrlRegNames::OPERATION)] = 10;
	data_regs[0] = 1;
	data_regs[1] = memorySize + 1;
	reg_file[1] = 12345;

	EXPECT_FALSE(execute());
}

TEST(ExecuteTest, LoadOperation){
  cntrl_regs[static_cast<int>(CntrlRegNames::OPERATION)] = 11;
  data_regs[0] = 1;
  data_regs[1] = 100;
  auto* address = reinterpret_cast<unsigned int*>(prog_mem + 100);
	*address = 54321;

  EXPECT_TRUE(execute());
	EXPECT_EQ(reg_file[1], 54321);
}

TEST(ExecuteTest, DivByZero){
  cntrl_regs[static_cast<int>(CntrlRegNames::OPERATION)] = 24;
  data_regs[0] = 1;
  data_regs[1] = 2;
	data_regs[2] = 3;
	reg_file[2] = 50;
	reg_file[3] = 0;	

  EXPECT_FALSE(execute());
}

int main(int argc, char **argv){
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
};
