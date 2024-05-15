_Pragma("once");
#include <string>
#include <unordered_map>
#include <vector>


struct CapacitorSpecification
{
    float capacitance;
    float current;
    std::string name;
    float power;
    float voltage;
};

class TankCalculator
{
    std::unordered_map<std::string, std::unique_ptr<CapacitorSpecification>> stored_specs;

public:
    TankCalculator(std::vector<CapacitorSpecification> &specs);
    void calculate_capacitors_tank(
        float frequency,
        float current,
        std::vector<std::string> &group1,
        std::vector<std::string> &group2);
};