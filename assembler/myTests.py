import pytest
import subprocess
import os

# Helper function to run the assembler script and catch SystemExit
def run_assembler(input_asm_file):
    process = subprocess.run(["python3", "asm4380.py", f"testFiles/{input_asm_file}"],
                             capture_output=True, text=True)
    print(process)
    return {
        "error": process.returncode != 0,
        "exit_code": process.returncode,
        "output": process.stdout if not process.returncode else process.stderr
    }

@pytest.mark.parametrize("input_asm, expected_exit_code", [
    ("invalidLabel.asm", 2),
    ("invalidLabel2.asm", 2),
		("invalidDirective.asm", 2),
		("invalidDirective2.asm", 2),
		("invalidInstr.asm", 2),
		("invalidInstr.asm", 2),
		("invalidCodeSection.asm", 2),
		("invalidDataSection.asm", 2),
		("invalidMain.asm", 2),
		("duplicateFunction.asm", 2),
		("missingLabel.asm", 2),
])

def test_assembler_errors(input_asm, expected_exit_code):
    result = run_assembler(input_asm)
    print(result)
    assert result["error"], f"Expected an error for {input_asm}, but got none."
    assert result["exit_code"] == expected_exit_code, f"Expected exit code {expected_exit_code} for {input_asm}, but got {result['exit_code']}."

@pytest.mark.parametrize("input_asm", [
		"valid1.asm",
		"valid2.asm",   
		"validArrayInit.asm",
		"simple.asm",
		"simple2.asm",
		"simple2Comments.asm",
		"validDirective.asm",
		"validDirective2.asm",
		"arithmetic.asm",
		"arithmetic2.asm",
		"movInstructions.asm",
		"movInstructions2.asm",
		"movInstructions3.asm",
		"trpInstructions.asm",
		"allInstructions.asm",
		"allInstructions2.asm",
		"prog_a.asm",
		"prog_b.asm",
		"prog_c.asm",
		"prog_d.asm",
		"prog_e.asm",
])

# Test case for the valid input file
def test_assembler_valid_output(input_asm):
		directory = "testFiles"

		expected_output_bin = os.path.join(directory, input_asm.replace('.asm', '.bin'))

		result = run_assembler(input_asm)

    # Assert no error occurred
		assert not result["error"], f"Did not expect an error for {input_asm}, but got one."

    # Assert the binary file exists
		assert os.path.exists(expected_output_bin), f"Expected binary file {expected_output_bin} does not exist."

    # Cleanup: Remove the generated binary file after the test
		os.remove(expected_output_bin)

