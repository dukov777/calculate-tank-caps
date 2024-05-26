#include <memory>
#include <string>
#include <array>
#include <cstring>
#include <stdexcept>

#include "Capacitors.hpp"
#include "MonitorDecorator.hpp"
#include "MemoryMngr.h"

#ifdef MEMORYPOOL


union Capacitors {
    void* address;
    Capacitor single_capacitor;
    ParallelCapacitors parallel_capacitor;
    MonitorDecorator monitor_decorator;
    SerialCapacitors serial_capacitor;

    // Define a default constructor for the union
    Capacitors() : single_capacitor("", 0, 0, 0, 0) {}
    ~Capacitors() {}
};

struct PoolElement
{
    union Capacitors cap;
    bool is_taken = false;
};

class CapacitorsPool
{
    std::array<PoolElement, 20> _pool;
public:
    static CapacitorsPool& get_instance();
    Capacitors& get_free_from_pool();
    void return_to_pool(void* element);
};


CapacitorsPool& 
CapacitorsPool::get_instance()
{
    static CapacitorsPool instance;
    return instance;
}

Capacitors& 
CapacitorsPool::get_free_from_pool()
{
    for(auto& element : _pool)
    {
        if(!element.is_taken)
        {
            element.is_taken = true;
            return element.cap;
        }
    }
    throw std::runtime_error("No free elements in the pool");
    exit(1);
}

void 
CapacitorsPool::return_to_pool(void* element)
{
    for(auto& pool_element : _pool)
    {
        if(&pool_element.cap == element && pool_element.is_taken)
        {
            pool_element.is_taken = false;
            memset(element, 0, sizeof(Capacitors));
            return;
        }
    }
}


void CapacitorDeleter::operator()(CapacitorInterface* ptr) const
{
    ptr->~CapacitorInterface();
    CapacitorsPool::get_instance().return_to_pool(ptr);
}

CapacitorPtr
CapacitorFactory::create_single(const std::string &name, float capacitance, float max_voltage, float max_current, float max_power)
{
    Capacitors& cap = CapacitorsPool::get_instance().get_free_from_pool();
    new (&cap.single_capacitor) Capacitor(name, capacitance, max_voltage, max_current, max_power);
    return CapacitorPtr(&cap.single_capacitor);
}

CapacitorPtr
CapacitorFactory::create_parallel(std::string name, std::vector<CapacitorPtr> &&caps)
{
    Capacitors& cap = CapacitorsPool::get_instance().get_free_from_pool();
    new (&cap.parallel_capacitor) ParallelCapacitors(name, std::move(caps));
    return CapacitorPtr(&cap.parallel_capacitor);
}

CapacitorPtr
CapacitorFactory::create_monitor_decorator(CapacitorPtr &&caps)
{
    Capacitors& cap = CapacitorsPool::get_instance().get_free_from_pool();
    new (&cap.monitor_decorator) MonitorDecorator(std::move(caps));
    return CapacitorPtr(&cap.monitor_decorator);
}

CapacitorPtr
CapacitorFactory::create_serial(std::string name, std::vector<CapacitorPtr> &&caps)
{
    Capacitors& cap = CapacitorsPool::get_instance().get_free_from_pool();
    new (&cap.serial_capacitor) SerialCapacitors(name, std::move(caps));
    return CapacitorPtr(&cap.serial_capacitor);
}

#else
void CapacitorDeleter::operator()(CapacitorInterface* ptr) const
{
    delete ptr;
}

CapacitorPtr
CapacitorFactory::create_single(const std::string &name, float capacitance, float max_voltage, float max_current, float max_power)
{
    return std::move(CapacitorPtr(new Capacitor(name, capacitance, max_voltage, max_current, max_power)));
}

CapacitorPtr 
CapacitorFactory::create_parallel(std::string name, std::vector<CapacitorPtr> &&caps)
{
    return std::move(CapacitorPtr(new ParallelCapacitors(name, std::move(caps))));
}

CapacitorPtr CapacitorFactory::create_monitor_decorator(CapacitorPtr &&cap)
{
    return std::move(CapacitorPtr(new MonitorDecorator(std::move(cap))));
}

CapacitorPtr CapacitorFactory::create_serial(std::string name, std::vector<CapacitorPtr> &&caps)
{
    return std::move(CapacitorPtr(new SerialCapacitors(name, std::move(caps))));
}

#endif