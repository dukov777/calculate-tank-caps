#include <vector>
#include <memory>
#include <numeric>
#include <unordered_map>
#include <iostream>
#include "Capacitors.hpp"
#include "MonitorDecorator.hpp"
#include "TankCalculator.hpp"
#include "MemoryMngr.h"

TankCalculator::TankCalculator(std::vector<CapacitorSpecification> &specs)
{
    for (auto &spec : specs)
    {
        stored_specs[spec.name] = std::make_unique<CapacitorSpecification>(spec);
    }
}

void TankCalculator::compose_capacitors_tank(
    std::vector<std::string> &group1,
    std::vector<std::string> &group2)
{
    std::vector<CapacitorPtr> caps1;
    std::vector<CapacitorPtr> caps2;

    _compose_caps(group1, caps1);
    _compose_caps(group2, caps2);

    auto parallel1 = CapacitorFactory::create_monitor_decorator(
        CapacitorFactory::create_parallel("group1", std::move(caps1)));

    auto parallel2 = CapacitorFactory::create_monitor_decorator(
        CapacitorFactory::create_parallel("group2", std::move(caps2)));

    std::vector<CapacitorPtr> serials;
    serials.push_back(std::move(parallel1));
    serials.push_back(std::move(parallel2));

    serial_tank = std::move(
        CapacitorFactory::create_monitor_decorator(std::move(
            CapacitorFactory::create_serial("Tank Circuit Example", std::move(serials)))));
}

void TankCalculator::_compose_caps(std::vector<std::string> &group, std::vector<CapacitorPtr> &caps)
{
    for (auto &name : group)
    {
        if (stored_specs.find(name) == stored_specs.end())
        {
            std::cerr << "Error: Capacitor " << name << " not found in the specification file." << std::endl;
            exit(EXIT_FAILURE);
        }

        auto cap = CapacitorFactory::create_single(
            stored_specs[name]->name,
            stored_specs[name]->capacitance,
            stored_specs[name]->voltage,
            stored_specs[name]->current,
            stored_specs[name]->power);
        auto cap_decorator = CapacitorFactory::create_monitor_decorator(std::move(cap));
        caps.push_back(std::move(cap_decorator));
    }
}

void TankCalculator::calculate_capacitors_tank(float frequency, float current)
{
    serial_tank->calculate(frequency, current);
}
