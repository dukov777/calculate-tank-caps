#include <vector>
#include <memory>
#include <numeric>
#include <unordered_map>

#include "Capacitors.hpp"
#include "MonitorDecorator.hpp"
#include "TankCalculator.hpp"


TankCalculator::TankCalculator(std::vector<CapacitorSpecification> &specs)
{
    for (auto &spec : specs)
    {
        stored_specs[spec.name] = std::make_unique<CapacitorSpecification>(spec);
    }
}

void TankCalculator::calculate_capacitors_tank(
    float frequency,
    float current,
    std::vector<std::string> &group1,
    std::vector<std::string> &group2)
{
    std::vector<std::unique_ptr<CapacitorInterface>> caps1;
    std::vector<std::unique_ptr<CapacitorInterface>> caps2;

    for (auto &name : group1)
    {
        caps1.push_back(
            std::make_unique<MonitorDecorator>(
                std::make_unique<Capacitor>(
                    stored_specs[name]->name,
                    stored_specs[name]->capacitance,
                    stored_specs[name]->voltage,
                    stored_specs[name]->current,
                    stored_specs[name]->power)));
    }

    for (auto &name : group2)
    {
        caps2.push_back(
            std::make_unique<MonitorDecorator>(
                std::make_unique<Capacitor>(
                    stored_specs[name]->name,
                    stored_specs[name]->capacitance,
                    stored_specs[name]->voltage,
                    stored_specs[name]->current,
                    stored_specs[name]->power)));
    }

    std::vector<std::unique_ptr<CapacitorInterface>> serials;
    // serials.push_back(std::make_unique<CurrentMonitorDecorator>(std::make_unique<SerialCapacitors>("Serial 1", std::move(caps1))));
    serials.push_back(std::make_unique<MonitorDecorator>(std::make_unique<ParallelCapacitors>("group1", std::move(caps1))));
    serials.push_back(std::make_unique<MonitorDecorator>(std::make_unique<ParallelCapacitors>("group2", std::move(caps2))));

    auto serial_tank = std::make_unique<MonitorDecorator>(std::make_unique<SerialCapacitors>("Tank Circuit Example", std::move(serials)));
    serial_tank->calculate(frequency, current);
}
