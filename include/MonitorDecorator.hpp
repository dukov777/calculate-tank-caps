#pragma once

#include <vector>

#include "Capacitors.hpp"
#include "MemoryMngr.h"

class MonitorDecorator : public CapacitorInterface
{
private:
    CapacitorPtr cap;

public:
    MonitorDecorator(CapacitorPtr&& cap)
        : cap(std::move(cap)) {}
    void calculate(float frequency, float current) override;
    std::string get_name() override { return cap->get_name(); }
    float get_current() override { return cap->get_current(); }
    float get_voltage() override { return cap->get_voltage(); }
    float get_power() override { return cap->get_power(); }
    float get_capacitance() override { return cap->get_capacitance(); }
    float get_xc() override { return cap->get_xc(); }
    CapacitorSpecification get_spec() override { return cap->get_spec(); }
    virtual float calculate_xc(float frequency) override { return cap->calculate_xc(frequency); }

    ~MonitorDecorator() {}
};
