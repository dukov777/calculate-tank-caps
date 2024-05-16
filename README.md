# calculate-tank-caps
Tank capacitor bank calculation software in C++.

## Requirements

[Software Requirement Specification](SoftwareRequirementSpecification.md)

## Design decisions

The code is organized with 2 intends: 
* Performance for the calculation part
* Simplicity for serving utility code - 
   * Input - command line reading, JSON file parsing, 
   * Output - dumping output directly to the console.
* The code does not impose various assertions and guards in calculations and relies on functional programming.

## Design
The capacitor circuit is assembled, and calculations are done in the `TankCalculator` class. The actual composition is made in the method `compose_capacitors_tank` using the Composition pattern. The capacitor calculation itself is executed in `calculate_capacitors_tank`. The results are directly dragged into the console by empowering decorators for each capacitor/group.  

The display of the calculations result and range exceedings is printed to the console. This is done by Decorating each capacitor/group and thus letting the decorator decide how to display results.    
This decision is made to keep the family of Capacitor classes clean and with a single responsibility: calculation, while result display and validation are delegated to others.

## Install
1. Clone the repo
2. Get the submodules

   `git submodule update`

4. Build the project 
```bash
  mkdir build
  cd build
  cmake ..
  make
```
4. Run the app

   `./calculate-tank-caps -i 10 -f 60 -group1 3uF_800V -group2 1uF_1000V 3uF_800V -spec ../capacitors-spec.json` 
