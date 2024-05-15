#include <argparse/argparse.hpp>
#include <iostream>
#include <string>
#include <vector>


int main(int argc, char** argv) {
    // Create an ArgumentParser object
    argparse::ArgumentParser program("calculate-tank-caps");

    program.add_argument("-i")
        .help("Current")
        .default_value(0.0f)
        .scan<'g', float>();

    program.add_argument("-f")
        .help("Frequency")
        .default_value(0.0f)
        .scan<'g', float>();

    program.add_argument("-group1")
        .help("Capacitors in group 1. Minimum 1 capacitors required. Maximum 5 capacitors allowed.")
        .nargs(1, 5)
        .action([](const std::string& value) { return value; });

    program.add_argument("-group2")
        .help("Capacitors in group 2. Minimum 1 capacitors required. Maximum 5 capacitors allowed.")
        .nargs(1, 5)
        .action([](const std::string& value) { return value; });

    try {
        // Parse the command line arguments
        program.parse_args(argc, argv);
    }
    catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        exit(EXIT_FAILURE);
    }

    auto i = program.get<float>("-i");
    std::cout << "i: " << i << std::endl;

    auto f = program.get<float>("-f");
    std::cout << "f: " << f << std::endl;
        
    auto group1 = program.get<std::vector<std::string>>("-group1"); // {1.95, 2.47}
    if (!group1.empty()) {
        std::cout << "group1: ";
        for (auto &v : group1) {
            std::cout << v << " ";
        }
        std::cout << std::endl;
    }

    auto group2 = program.get<std::vector<std::string>>("-group2");
    if (!group2.empty()) {
        std::cout << "group2: ";
        for (auto &v : group2) {
            std::cout << v << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
