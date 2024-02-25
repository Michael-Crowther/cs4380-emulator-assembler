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
    ("missing#.asm", 2),
])

def test_assembler_errors(input_asm, expected_exit_code):
    result = run_assembler(input_asm)
    print(result)
    assert result["error"], f"Expected an error for {input_asm}, but got none."
    assert result["exit_code"] == expected_exit_code, f"Expected exit code {expected_exit_code} for {input_asm}, but got {result['exit_code']}."

@pytest.mark.parametrize("input_asm", [
		"valid1.asm",
		"valid2.asm",    
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
