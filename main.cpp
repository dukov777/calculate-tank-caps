#include <argparse/argparse.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <nlohmann/json.hpp>

#include "calculate-capacitors.hpp"

using json = nlohmann::json;

struct ProgramData
{
    float i;
    float f;
    std::vector<std::string> group1;
    std::vector<std::string> group2;
    std::string capacitor_spec_file;
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

    program.add_argument("-spec")
        .help("Path to capacitor specification file")
        .default_value(std::string("capacitors-spec.json"));

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

    data.capacitor_spec_file = program.get<std::string>("-spec");

    return data;
}


CapacitorSpecification parseComponent(const json &j)
{
    CapacitorSpecification comp;
    comp.capacitance = j.at("capacitance").get<float>();
    comp.current = j.at("current").get<float>();
    comp.name = j.at("name").get<std::string>();
    comp.power = j.at("power").get<float>();
    comp.voltage = j.at("voltage").get<float>();
    return comp; // Use std::move to enable move semantics
}


std::vector<CapacitorSpecification> parseCapacitorSpecifications(const std::string &filepath)
{
    std::vector<CapacitorSpecification> capacitor_spec;

    // read the capacitor specification file
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open capacitor specification file." << std::endl;
        exit(EXIT_FAILURE);
    }

    // parse the json data
    json json_data;
    try
    {
        file >> json_data;
    }
    catch (json::parse_error &e)
    {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return capacitor_spec;
    }

    try
    {
        for (const auto &item : json_data)
        {
            capacitor_spec.emplace_back(parseComponent(item));
        }
    }
    catch (json::exception &e)
    {
        std::cerr << "Error parsing capacitor specification file: " << e.what() << std::endl;
        return capacitor_spec;
    }

    return capacitor_spec;
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

    std::vector<CapacitorSpecification> capacitor_spec = parseCapacitorSpecifications(data.capacitor_spec_file);

    // Calculate the tank capacitors
    TankCalculator tank_calculator(capacitor_spec);
    tank_calculator.calculate_capacitors_tank(data.f, data.i, data.group1, data.group2);
    
    
    return 0;
}
