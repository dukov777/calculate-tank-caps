_Pragma("once");
#include <string>
#include <unordered_map>
#include <vector>


struct CapacitorSpecification
{
    float capacitance;
    float current;
    std::string name;
    float power;
    float voltage;
};

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


class Capacitor : public CapacitorBase
{
public:
    Capacitor(const std::string &name, float capacitance, float max_voltage, float max_current, float max_power)
        : CapacitorBase(name, capacitance, max_voltage, max_current, max_power) {}

    void calculate(float frequency, float current) override;
    ~Capacitor() {}
};

class SerialCapacitors : public CapacitorBase
{
private:
    std::vector<std::unique_ptr<CapacitorInterface>> caps;

    float totalCapacitanceSeries();

public:
    SerialCapacitors(std::string name, std::vector<std::unique_ptr<CapacitorInterface>> &&caps);
    void calculate(float frequency, float current) override;
    ~SerialCapacitors() {}
};


class ParallelCapacitors : public CapacitorBase
{
private:
    std::vector<std::unique_ptr<CapacitorInterface>> caps;
    float totalCapacitanceParallel();
public:
    ParallelCapacitors(std::string name, std::vector<std::unique_ptr<CapacitorInterface>> &&caps);
    void calculate(float frequency, float current) override;
    ~ParallelCapacitors() {};
};

class TankCalculator
{
    std::unordered_map<std::string, std::unique_ptr<CapacitorSpecification>> stored_specs;

public:
    TankCalculator(std::vector<CapacitorSpecification> &specs);
    void calculate_capacitors_tank(
        float frequency,
        float current,
        std::vector<std::string> &group1,
        std::vector<std::string> &group2);
};
