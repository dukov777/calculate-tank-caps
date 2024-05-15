#include <iostream>

#include "MonitorDecorator.hpp"
#include "Capacitors.hpp"


void MonitorDecorator::calculate(float frequency, float current)
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
};
