**Software Requirement Specification**

**1. Introduction**
This document specifies the requirements for the Tank Capacitor Group calculation software. Based on user inputs, the software calculates electrical parameters for a tank capacitor group configuration.

**2. Functional Requirements**

**2.1 User Input**
- FR-01: The software shall allow the user to input the type of capacitors. The capacitors are passed as command-line parameters as follows:
  - For capacitor group 1: `"-group1  23uF_500V 1uF_500V"`
  - For capacitor group 2: `"-group2  23uF_500V 10uF_500V"`
  
  Example: Run the app with all capacitors:
  ```shell
  <app> -group1 23uF_500V 1uF_500V -group2 23uF_500V 10uF_500V
  ```

- FR-02: The software shall allow the user to choose how many capacitors they require per group. The allowed range is a minimum of 1 to a maximum of 5.

- FR-03: The software shall allow the user to input the frequency of the current source A:
  ```shell
  <app> -f 60
  ```

- FR-04: The software shall allow the user to input the RMS value of the current of A (in amperes):
  ```shell
  <app> -i 1.5
  ```

- FR-05: The software shall read the type of capacitors from a JSON-encoded file. All the real numbers are in the SI system. The capacitors are in Farads; for example, a capacitor of 10uF is represented as 10e-6. The file format is as:
    ```JSON
      [
        {
          "name": "23uF_500V",
          "capacitance": 23e-6,
          "voltage": 500,
          "current": 1000,
          "power": 500e+3
        }
      ]

    ```

- FR-06: The software shall receive the path to the capacitor JSON encoded file as input.

**2.2 Calculations**
- FR-07: The software shall calculate the RMS current through each capacitor.
- FR-08: The software shall calculate the RMS voltage across each capacitor.
- FR-09: The software shall calculate the maximum RMS value of the allowable current, voltage, and reactive power through the tank capacitor group, considering the allowed current, voltage, and reactive power.


**2.3 Output**
- FR-10: The software shall display the RMS current through each capacitor.
- FR-11: The software shall display the RMS voltage across each capacitor.
- FR-12: The software shall display the maximum RMS value of the allowable current through the tank capacitor group. A warning message should be displayed if any capacitor parameters are exceeded. The warning should explicitly state which capacitor group and parameters have been exceeded.

    Example warnings:
    ```shell
    Warning: Overvoltage condition on capacitor group 1. The voltage is 2000V, which exceeds the maximum voltage of 500V!
    Warning: Overcurrent condition on capacitor group 1. The current is 1100A, which exceeds the maximum current of 100A!
    Warning: Overloading condition on capacitor group 1. The reactive power is 600kVA, which exceeds the maximum power of 500kVA!
    ```

**3. Non-Functional Requirements**

**3.1 Performance**
NA

**3.2 Usability**
- NFR-01: The software shall provide a command-line interface (CLI).

**3.3 Reliability**
- NFR-02: The software shall handle invalid user inputs gracefully and provide appropriate error messages.

**3.4 Scalability**
NA

**3.5 Portability**
- NFR-03: The software shall be compatible with Linux operating systems.

**3.6 Maintainability**
- NFR-04: The software shall be developed in C++ with well-documented code.

**4. Constraints**
- CON-01: The maximum number of capacitors in a group is 5.
- CON-02: The minimum number of capacitors in a group is 1.
- CON-03: The number of groups is 2.

**5. Assumptions**
- ASM-01: Users are familiar with basic electrical engineering concepts.
