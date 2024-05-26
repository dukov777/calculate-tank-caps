#pragma once

#include <memory>
#include <string>
#include <array>
#include <cstring>
#include <stdexcept>

class CapacitorInterface;

class CapacitorDeleter {
public:
    void operator()(CapacitorInterface* ptr) const;
};


typedef std::unique_ptr<CapacitorInterface, CapacitorDeleter> CapacitorPtr;

class CapacitorFactory
{
public:
    static CapacitorPtr
    create_single(const std::string &name, float capacitance, float max_voltage, float max_current, float max_power);

    static CapacitorPtr
    create_parallel(std::string name, std::vector<CapacitorPtr> &&caps);

    static CapacitorPtr
    create_monitor_decorator(CapacitorPtr &&cap);

    static CapacitorPtr
    create_serial(std::string name, std::vector<CapacitorPtr> &&caps);

};

