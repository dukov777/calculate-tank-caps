#include <argparse/argparse.hpp>
#include <iostream>
#include <string>
#include <vector>


struct ProgramData
{
    float i;
    float f;
    std::vector<std::string> group1;
    std::vector<std::string> group2;
};

ProgramData get_commnad_line_params(int argc, char **argv)
{
    ProgramData data;

    // create a new ArgumentParser object
    argparse::ArgumentParser program("calculate-tank-caps");

    // add program arguments
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
        .action([](const std::string &value)
                { return value; });

    program.add_argument("-group2")
        .help("Capacitors in group 2. Minimum 1 capacitors required. Maximum 5 capacitors allowed.")
        .nargs(1, 5)
        .action([](const std::string &value)
                { return value; });

    try
    {
        // Parse the command line arguments
        program.parse_args(argc, argv);
    }
    catch (const std::runtime_error &err)
    {
        // dump help message and exit
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        exit(EXIT_FAILURE);
    }

    // Get the CLI values
    data.i = program.get<float>("-i");
    data.f = program.get<float>("-f");
    data.group1 = program.get<std::vector<std::string>>("-group1");
    data.group2 = program.get<std::vector<std::string>>("-group2");

    return data;
}

int main(int argc, char **argv)
{
    // get the command line parameters
    ProgramData data = get_commnad_line_params(argc, argv);

    // validate constraints on the input data
    if (data.group1.size() < 1 || data.group1.size() > 5)
    {
        std::cerr << "Error: Capacitors in group 1. Minimum 1 capacitors required. Maximum 5 capacitors allowed." << std::endl;
        exit(EXIT_FAILURE);
    }

    if (data.group2.size() < 1 || data.group2.size() > 5)
    {
        std::cerr << "Error: Capacitors in group 2. Minimum 1 capacitors required. Maximum 5 capacitors allowed." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Output the data just for clarity
    std::cout << "i: " << data.i << std::endl;
    std::cout << "f: " << data.f << std::endl;
    std::cout << "group1: ";
    for (auto &v : data.group1)
    {
        std::cout << v << " ";
    }
    std::cout << std::endl;

    std::cout << "group2: ";
    for (auto &v : data.group2)
    {
        std::cout << v << " ";
    }
    std::cout << std::endl;

    return 0;
}
