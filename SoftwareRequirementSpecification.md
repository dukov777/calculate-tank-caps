**Software Requirement Specification**

**1. Introduction**

This document specifies the requirements for the Tank Capacitor Group calculation software. The software calculates electrical parameters for a tank capacitor group configuration based on user inputs.

**2. Functional Requirements**

2.1 **User Input**
- FR-01: The software shall allow the user to input the type of capacitors. The capacitors are enumerated and passeed as command line parameters as follows:
    for capacitor group 1:
        
        ` "--c1", "--c2", "--c3", "--c4", "--c5" `
    
    for capacitor group 2:
        
        ` "--c6", "--c7", "--c8", "--c9", "--c10" `
    
    Example: Run the app with all capacitors
        
        `<app> -c1 -c2 -c3 -c4 -c5 -c6 -c7 -c8 -c9 -c10`

    
- FR-02: The software shall allow the user to choose how many capacitors he requires and if he requires less than 5 capacitors per capacitor group the software to omit all unused.
    
    Example: Use only 2 capacitors from capacitor group 1 and capacitor group 2
    
    `<app> -c1 -c2 -c6 -c7`

    Example: Error if only one capacitor group of mandatory 2 is used. Use only 2 capacitors from capacitor group 1 and capacitor group 2
    
    `<app> -c1 -c2`
    `Error: Capacitor group 2 is missing. List atleast one capacitor from capacitor group 2`


- FR-03: The software shall allow the user to input the frequency of the current source A.
    
    `<app> -f 60`

- FR-04: The software shall allow the user to input the RMS value of the current of A. The input is in ampers.
    
    `<app> -v 1.5`

- FR-05: The software shall read the type of capacitors from JSON encoded file.
- FR-06: The software shall receive as input the path to capacitors JSON encoded file.

2.2 **Calculations**
- FR-07: The software shall calculate the RMS current through each capacitor.
- FR-08: The software shall calculate the RMS voltage across each capacitor.
- FR-09: The software shall calculate the maximum RMS value of the allowable current, voltage and reactive power through the tank capacitor group. Allowed current, voltage and reactive power should be considered by respective capacitors types.


2.3 **Output**
- FR-10: The software shall display the equivalent capacitance of the group.
- FR-11: The software shall display the RMS current through each capacitor.
- FR-12: The software shall display the RMS voltage across each capacitor.
- FR-13: The software shall display the maximum RMS value of the allowable current through the tank capacitor group. If any of the capacitor parameters is exceeded a warning message should be displayed. The warning should explicetelly state which capacitor group and parameters are exceeded.
    
    * `Warning: Overvoltage conditionon on capacitor group 1. The voltage is 2000V which exceeds maximum voltage of 500V!`
    
    * `Warning: Overcurrent conditionon on capacitor group 1. The current is 1100A which exceeds maximum current of 100A!`
    
    * `Warning: Overloading conditionon on capacitor group 1. The reactive power is 600kVA which exceeds maximum power of 500kVA!`

**3. Non-Functional Requirements**

3.1 **Performance**
    NA

3.2 **Usability**
- NFR-01: The software shall provide a command-line interface (CLI).

3.3 **Reliability**
- NFR-02: The software shall handle invalid user inputs gracefully and provide appropriate error messages.

3.4 **Scalability**
    NA

3.5 **Portability**
- NFR-03: The software shall be compatible with Linux operating systems.

3.6 **Maintainability**
- NFR-04: The software shall be developed in C++ with well-documented code.

**4. Constraints**

- CON-01: The maximum number of capacitors in a group is 5.
- CON-02: The minimum number of capacitors in a group is 1.
- CON-03: The number of groups is 2.

**5. Assumptions**

- ASM-01: Users are familiar with basic electrical engineering concepts.
- ASM-02: Users have access to a system with a compatible operating system.

