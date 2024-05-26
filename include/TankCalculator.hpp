#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include "Capacitors.hpp"
#include "MonitorDecorator.hpp"

class TankCalculator
{
    std::unordered_map<std::string, std::unique_ptr<CapacitorSpecification>> stored_specs;
    CapacitorPtr serial_tank;
public:
    TankCalculator(std::vector<CapacitorSpecification> &specs);
    void compose_capacitors_tank(std::vector<std::string> &group1, std::vector<std::string> &group2);
    void calculate_capacitors_tank(float frequency, float current);
};
