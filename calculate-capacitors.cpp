#include <iostream>
#include <vector>
#include <memory> // Include for smart pointers
#include <numeric>
#include <unordered_map>

#include "calculate-capacitors.hpp"

class CapacitorInterface
{
public:
    virtual void calculate(float frequency, float current) = 0;
    virtual float calculate_xc(float frequency) = 0;
    virtual ~CapacitorInterface() {}
    virtual std::string get_name() = 0;
    virtual float get_current() = 0;
    virtual float get_voltage() = 0;
    virtual float get_power() = 0;
    virtual float get_capacitance() = 0;
    virtual float get_xc() = 0;
    virtual CapacitorSpecification get_spec() = 0;
};

class CapacitorBase : public CapacitorInterface
{
protected:
    std::string name;
    float capacitance;
    float max_voltage;
    float max_current;
    float max_power;

    float xc;
    float current;
    float voltage;
    float power;

public:
    CapacitorBase(const std::string &name, float capacitance, float max_voltage, float max_current, float max_power)
        : name(name), capacitance(capacitance), max_voltage(max_voltage), max_current(max_current), max_power(max_power) {}

    std::string get_name() override { return name; }
    float get_current() override { return current; }
    float get_voltage() override { return voltage; }
    float get_power() override { return power; }
    float get_capacitance() override { return capacitance; }
    float get_xc() override { return xc; }
    CapacitorSpecification get_spec() override { return CapacitorSpecification{capacitance, max_current, name, max_power, max_voltage}; }
    float calculate_xc(float frequency) override;
    virtual ~CapacitorBase() {}
};

float CapacitorBase::calculate_xc(float frequency)
{
    xc = 1 / (2 * 3.14159 * frequency * capacitance);
    return xc;
}


class Capacitor : public CapacitorBase
{
public:
    Capacitor(const std::string &name, float capacitance, float max_voltage, float max_current, float max_power)
        : CapacitorBase(name, capacitance, max_voltage, max_current, max_power) {}

    void calculate(float frequency, float current) override
    {
        xc = calculate_xc(frequency);
        voltage = xc * current;
        power = voltage * current;
        this->current = current;
    }


    ~Capacitor() {}
};

class SerialCapacitors : public CapacitorBase
{
private:
    std::vector<std::unique_ptr<CapacitorInterface>> caps;

    float totalCapacitanceSeries()
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

public:
    SerialCapacitors(std::string name, std::vector<std::unique_ptr<CapacitorInterface>> &&caps)
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

    void calculate(float frequency, float current) override
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

    ~SerialCapacitors() {}
};


class ParallelCapacitors : public CapacitorBase
{
private:
    std::vector<std::unique_ptr<CapacitorInterface>> caps;

    float totalCapacitanceParallel()
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

public:
    ParallelCapacitors(std::string name, std::vector<std::unique_ptr<CapacitorInterface>> &&caps)
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

    void calculate(float frequency, float current) override
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
};


class CurrentMonitorDecorator : public CapacitorInterface
{
private:
    std::unique_ptr<CapacitorInterface> cap;

public:
    CurrentMonitorDecorator(std::unique_ptr<CapacitorInterface> cap)
        : cap(std::move(cap)) {}

    void calculate(float frequency, float current) override
    {
        cap->calculate(frequency, current);
        std::cout << "Capacitor: " << cap->get_name()
                  << ", Current: " << cap->get_current()
                  << ", Voltage: " << cap->get_voltage()
                  << ", Power: " << cap->get_power()
                  << "\n";
        
        if (cap->get_current() > cap->get_spec().current)
        {
            // Message format is Warning: Overcurrent condition on capacitor group 1. The current is 1100A, which exceeds the maximum current of 100A!
            std::cout << "Warning: Overcurrent condition on " << cap->get_name() << ". The current is " << cap->get_current() << "A, which exceeds the maximum current of " << cap->get_spec().current << "A!\n";
        }
        if (cap->get_voltage() > cap->get_spec().voltage)
        {
            // Message format is Warning: Overvoltage condition on capacitor group 1. The voltage is 1100V, which exceeds the maximum voltage of 100V!
            std::cout << "Warning: Overvoltage condition on " << cap->get_name() << ". The voltage is " << cap->get_voltage() << "V, which exceeds the maximum voltage of " << cap->get_spec().voltage << "V!\n";
        }

        if(cap->get_power() > cap->get_spec().power)
        {
            std::cout << "Warning: Overpower condition on " << cap->get_name() << ". The power is " << cap->get_power() << "W, which exceeds the maximum power of " << cap->get_spec().power << "W!\n";
        }
    }

    std::string get_name() override { return cap->get_name(); }
    float get_current() override { return cap->get_current(); }
    float get_voltage() override { return cap->get_voltage(); }
    float get_power() override { return cap->get_power(); }
    float get_capacitance() override { return cap->get_capacitance(); }
    float get_xc() override { return cap->get_xc(); }
    CapacitorSpecification get_spec() override { return cap->get_spec(); }
    virtual float calculate_xc(float frequency) override { return cap->calculate_xc(frequency); }

    ~CurrentMonitorDecorator() {}
};

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
            std::make_unique<CurrentMonitorDecorator>(
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
            std::make_unique<CurrentMonitorDecorator>(
                std::make_unique<Capacitor>(
                    stored_specs[name]->name,
                    stored_specs[name]->capacitance,
                    stored_specs[name]->voltage,
                    stored_specs[name]->current,
                    stored_specs[name]->power)));
    }

    std::vector<std::unique_ptr<CapacitorInterface>> serials;
    // serials.push_back(std::make_unique<CurrentMonitorDecorator>(std::make_unique<SerialCapacitors>("Serial 1", std::move(caps1))));
    serials.push_back(std::make_unique<CurrentMonitorDecorator>(std::make_unique<ParallelCapacitors>("group1", std::move(caps1))));
    serials.push_back(std::make_unique<CurrentMonitorDecorator>(std::make_unique<ParallelCapacitors>("group2", std::move(caps2))));

    auto serial_tank = std::make_unique<CurrentMonitorDecorator>(std::make_unique<SerialCapacitors>("Tank Circuit Example", std::move(serials)));
    serial_tank->calculate(frequency, current);
}

// void calculate_capacitors_tank(float frequency, float current, )
// {
//     std::vector<std::unique_ptr<CapacitorInterface>> caps1;
//     for(auto &spec : specs)
//     {
//         caps1.push_back(
//             std::make_unique<CurrentMonitorDecorator>(
//             std::make_unique<Capacitor>(spec.name, spec.capacitance, spec.voltage, spec.current, spec.power)));
//     }

//     caps1.push_back(std::make_unique<CurrentMonitorDecorator>(std::make_unique<Capacitor>("C1", 1e-6, 1000, 100, 500e3)));
//     caps1.push_back(std::make_unique<CurrentMonitorDecorator>(std::make_unique<Capacitor>("C2", 2e-6, 1000, 100, 500e3)));

//     std::vector<std::unique_ptr<CapacitorInterface>> serials;
//     serials.push_back(std::make_unique<CurrentMonitorDecorator>(std::make_unique<SerialCapacitors>("Serial 1", std::move(caps1))));

//     std::vector<std::unique_ptr<CapacitorInterface>> caps2;
//     caps2.push_back(std::make_unique<CurrentMonitorDecorator>(std::make_unique<Capacitor>("C3", 1e-6, 1000, 100, 500e3)));
//     caps2.push_back(std::make_unique<CurrentMonitorDecorator>(std::make_unique<Capacitor>("C4", 2e-6, 1000, 100, 500e3)));
//     serials.push_back(std::make_unique<CurrentMonitorDecorator>(std::make_unique<SerialCapacitors>("Serial 2", std::move(caps2))));

//     auto serial_tank = std::make_unique<CurrentMonitorDecorator>(std::make_unique<SerialCapacitors>("Tank Circuit Example", std::move(serials)));
//     serial_tank->calculate(60, 1);

//     // std::cout << "Total Voltage: " << serial_tank->getVoltage() << "\n";
//     // std::cout << "Total Power: " << serial_tank->getPower() << "\n";
//     // std::cout << "Total Xc: " << serial_tank->get_xc() << "\n";
//     // std::cout << "Total Current: " << serial_tank->getCurrent() << "\n";
// }
