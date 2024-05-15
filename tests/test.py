import subprocess

def run_cli_app():
    # Then, run the CLI application and capture the output
    cli_result = subprocess.run(
        ["./calculate-tank-caps", "-i", "2", "-f", "60",
         "-group1", "1uF_1000V", "3.3uF_800V", "-group2", "1uF_1000V", "3.3uF_800V", "-spec", "../capacitors-spec.json"],
        capture_output=True,
        text=True
    )

    return cli_result.stdout

def compare_output(expected_output, actual_output):
    # Compare the actual output with the expected output
    return expected_output.strip() == actual_output.strip()

def main():
    # The expected output
    expected_output = """Capacitor: 1uF_1000V, Current: 0.465116, Voltage: 1233.76, Power: 573.842
Warning: Overvoltage condition on 1uF_1000V. The voltage is 1233.76V, which exceeds the maximum voltage of 1000V!
Capacitor: 3.3uF_800V, Current: 1.53488, Voltage: 1233.76, Power: 1893.68
Warning: Overvoltage condition on 3.3uF_800V. The voltage is 1233.76V, which exceeds the maximum voltage of 800V!
Capacitor: group1, Current: 2, Voltage: 1233.76, Power: 2467.52
Warning: Overvoltage condition on group1. The voltage is 1233.76V, which exceeds the maximum voltage of 800V!
Capacitor: 1uF_1000V, Current: 0.465116, Voltage: 1233.76, Power: 573.842
Warning: Overvoltage condition on 1uF_1000V. The voltage is 1233.76V, which exceeds the maximum voltage of 1000V!
Capacitor: 3.3uF_800V, Current: 1.53488, Voltage: 1233.76, Power: 1893.68
Warning: Overvoltage condition on 3.3uF_800V. The voltage is 1233.76V, which exceeds the maximum voltage of 800V!
Capacitor: group2, Current: 2, Voltage: 1233.76, Power: 2467.52
Warning: Overvoltage condition on group2. The voltage is 1233.76V, which exceeds the maximum voltage of 800V!
Capacitor: Tank Circuit Example, Current: 2, Voltage: 2467.52, Power: 4935.04
Warning: Overvoltage condition on Tank Circuit Example. The voltage is 2467.52V, which exceeds the maximum voltage of 1600V!
"""

    try:
        actual_output = run_cli_app()

        if compare_output(expected_output, actual_output):
            print("Output matches expected output.")
        else:
            print("Output does not match expected output.")
            print("Expected Output:")
            print(expected_output)
            print("Actual Output:")
            print(actual_output)
    except RuntimeError as e:
        print(str(e))

if __name__ == "__main__":
    main()
