#include <vector>
#include <numeric>

#include "Capacitors.hpp"


float CapacitorBase::calculate_xc(float frequency)
{
    xc = 1 / (2 * 3.14159 * frequency * capacitance);
    return xc;
}

void Capacitor::calculate(float frequency, float current)
{
    xc = calculate_xc(frequency);
    voltage = xc * current;
    power = voltage * current;
    this->current = current;
}

float SerialCapacitors::totalCapacitanceSeries()
{
    float inverseTotal = 0.0;
    for (auto &cap : this->caps)
    {
        if (cap->get_capacitance() != 0)
        {
            inverseTotal += 1.0 / cap->get_capacitance();
        }
    }

    if (inverseTotal == 0.0)
    {
        return 0.0;
    }

    return 1.0 / inverseTotal;
}


SerialCapacitors::SerialCapacitors(std::string name, std::vector<std::unique_ptr<CapacitorInterface>> &&caps)
    : CapacitorBase(name, 0, 0, 0, 0)
{
    this->caps = std::move(caps);

    capacitance = totalCapacitanceSeries();

    // calculate the allowed maximum current through the group.
    // Maximum current is equal to the capacitor in the series with the smallest allowed current.
    max_current = (*std::min_element(
        this->caps.begin(), 
        this->caps.end(),
        [](const auto &a, const auto &b)
        {
            return a->get_spec().current < b->get_spec().current;
        }))->get_spec().current;

    // For series of capacitors the total allowed voltage is the sum of all capacitors maximum voltage
    max_voltage = std::accumulate(
        this->caps.begin(), 
        this->caps.end(),
        0.0,
        [](const auto &a, const auto &b)
        {
            return a + b->get_spec().voltage;
        });

    // For series of capacitors the total allowed power is the sum of all capacitors maximum power
    max_power = std::accumulate(
        this->caps.begin(), 
        this->caps.end(),
        0.0,
        [](const auto &a, const auto &b)
        {
            return a + b->get_spec().power;
        });
}

void SerialCapacitors::calculate(float frequency, float current)
{
    float total_voltage = 0;
    float total_power = 0;
    float total_xc = 0;

    for (auto &cap : caps)
    {
        cap->calculate(frequency, current);
        total_voltage += cap->get_voltage();
        total_power += cap->get_power();
        total_xc += cap->get_xc();
    }

    this->voltage = total_voltage;
    this->power = total_power;
    this->xc = total_xc;
    this->current = current;
}

float ParallelCapacitors::totalCapacitanceParallel()
{
    return std::accumulate(
        this->caps.begin(), 
        this->caps.end(),
        0.0,
        [](const auto &a, const auto &b)
        {
            return a + b->get_capacitance();
        });
}

ParallelCapacitors::ParallelCapacitors(std::string name, std::vector<std::unique_ptr<CapacitorInterface>> &&caps)
    : CapacitorBase(name, 0, 0, 0, 0)
{
    this->caps = std::move(caps);

    capacitance = totalCapacitanceParallel();

    // calculate the allowed maximum current through the group.
    // Maximum current is equal to the sum of each capacitor max current.
    max_current = std::accumulate(
        this->caps.begin(), 
        this->caps.end(),
        0.0,
        [](const auto &a, const auto &b)
        {
            return a + b->get_spec().current;
        });
    
    // For parallel capacitor circuit the maximum allowed voltage is the smallest maximum voltage of a capacitor in the group
    max_voltage = (*std::min_element(
        this->caps.begin(), 
        this->caps.end(),
        [](const auto &a, const auto &b)
        {
            return a->get_spec().voltage < b->get_spec().voltage;
        }))->get_spec().voltage;

    // For parallel capacitor circuit the total allowed power is the sum of all capacitors maximum power
    max_power = std::accumulate(
        this->caps.begin(), 
        this->caps.end(),
        0.0,
        [](const auto &a, const auto &b)
        {
            return a + b->get_spec().power;
        });
}

void ParallelCapacitors::calculate(float frequency, float current)
{
    float total_voltage = 0;
    float total_power = 0;
    float total_xc = 0;
    float total_reciprocal_xc = 0;

    for (auto &cap : caps)
    {
        total_reciprocal_xc += 1 / cap->calculate_xc(frequency);
    }

    total_xc = 1 / total_reciprocal_xc;      // Total reactance of the parallel group

    total_voltage = current * total_xc;     // Voltage across capacitors

    // once we have the voltage over parallel capacitors, we can calculate the current across each capacitor
    for (auto &cap : caps)
    {   
        // Calculate the current through each capacitor in the parallel group
        auto _current = total_voltage / cap->get_xc();
        // Calculate the voltage across each capacitor in the parallel group
        cap->calculate(frequency, _current);
        total_power += cap->get_power();
    }

    this->voltage = total_voltage;
    this->power = total_power;
    this->xc = total_xc;
    this->current = current;
}
