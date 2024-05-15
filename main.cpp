#include <argparse/argparse.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;


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

#include <string>

struct Component {
    std::string capacitance;
    std::string current;
    std::string name;
    std::string power;
    std::string voltage;
};

Component parseComponent(const json& j) {
    Component comp;
    comp.capacitance = j.at("capacitance").get<std::string>();
    comp.current = j.at("current").get<std::string>();
    comp.name = j.at("name").get<std::string>();
    comp.power = j.at("power").get<std::string>();
    comp.voltage = j.at("voltage").get<std::string>();
    return comp; // Use std::move to enable move semantics
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


    std::ifstream file("data.json");
    if (!file.is_open()) {
        std::cerr << "Could not open the file." << std::endl;
        return 1;
    }

    json jsonData;
    try {
        file >> jsonData;
    } catch (json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return 1;
    }

    std::vector<Component> components;
    try {
        for (const auto& item : jsonData) {
            components.emplace_back(parseComponent(item));
        }
    } catch (json::exception& e) {
        std::cerr << "Error parsing components: " << e.what() << std::endl;
        return 1;
    }
    
    // Output the data just for verification
    std::cout << "Components Loaded:" << std::endl;
    for (const auto& comp : components) {
        std::cout << "Name: " << comp.name << ", Voltage: " << comp.voltage << ", Current: " << comp.current << ", Power: " << comp.power << std::endl;
    }


    return 0;
}
