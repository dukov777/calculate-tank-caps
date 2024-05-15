# calculate-tank-caps
Tank capacitor bank calculation software in C++.

## Requirements

[Software Requirement Specification](SoftwareRequirementSpecification.md)


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
