#pragma once

#include <vector>

#include "Capacitors.hpp"

class MonitorDecorator : public CapacitorInterface
{
private:
    std::unique_ptr<CapacitorInterface> cap;

public:
    MonitorDecorator(std::unique_ptr<CapacitorInterface> cap)
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
