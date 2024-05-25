import math


class CapacitorSpec:
    def __init__(self, cap_uF, v_max, i_max, power_max):
        self.__cap_uF = cap_uF
        self.__cap_F = cap_uF * 1e-6
        self.__i_max = i_max
        self.__v_max = v_max
        self.__power_max = power_max
    
    get_i_max = lambda self: self.__i_max
    get_v_max = lambda self: self.__v_max
    get_power_max = lambda self: self.__power_max
    get_cap_F = lambda self: self.__cap_F
    get_cap_uF = lambda self: self.__cap_uF
        

class CapacitorInterface:
    def xc(self, f): pass
    def current(self, f, voltage): pass
    def voltage(self, f, current): pass
    def spec(self): pass
    def name(self): pass
    def allowed_current(self, f): pass
    

class Capacitor(CapacitorInterface):
    def __init__(self, cap_uF, vmax, imax, power_max=0.0, cap_name=""):
        self._spec = CapacitorSpec(cap_uF, vmax, imax, power_max)
        if cap_name == "":
            self._cap_name = f"{cap_uF:.0f}uF/{vmax:.0f}V"
        else:
            self._cap_name = cap_name
    
    xc = lambda self, f: 1 / (2 * math.pi * f * self._spec.get_cap_F())
    
    current = lambda self, f, voltage: voltage / self.xc(f)
    
    voltage = lambda self, f, current: current * self.xc(f)
    
    name = lambda self: self._cap_name
    
    spec = lambda self: self._spec

    allowed_current = lambda self, f: self._spec.get_v_max() / self.xc(f)
    

class ParallelCapacitor(Capacitor):
    def __init__(self, capacitors, cap_name=""):
        cap_uF = sum([cap.spec().get_cap_uF() for cap in capacitors])
        vmax = min([cap.spec().get_v_max() for cap in capacitors])
        imax = sum([cap.spec().get_i_max() for cap in capacitors])
        power_max = sum([cap.spec().get_power_max() for cap in capacitors])
        
        if cap_name == "":
            cap_name = f"parallel group {cap_uF:.0f}uF/{vmax:.0f}V"

        super().__init__(cap_uF, vmax, imax, power_max, cap_name)
        self._capacitors = capacitors
    
    def xc(self, f):
        reciprocal = sum([1 / cap.xc(f) for cap in self._capacitors])
        return 1 / reciprocal
    
    def current(self, f, voltage):
        # 1st law of Kirchhoff
        current = sum([cap.current(f, voltage) for cap in self._capacitors])
        return current

    def allowed_current(self, f):
        vmax = self.spec().get_v_max()
        return sum([cap.current(f, vmax) for cap in self._capacitors])
    

class SeriesCapacitor(Capacitor):
    def __init__(self, capacitors, cap_name=""):
        cap_uF = 1.0 / sum([1.0 /cap.spec().get_cap_uF() for cap in capacitors])
        vmax = sum([cap.spec().get_v_max() for cap in capacitors])
        imax = min([cap.spec().get_i_max() for cap in capacitors])
        power_max = sum([cap.spec().get_power_max() for cap in capacitors])
        self._spec = CapacitorSpec(cap_uF, vmax, imax, power_max)
        
        if cap_name == "":
            self._cap_name = f"serial group {cap_uF:.0f}uF/{vmax:.0f}V"
        
        super().__init__(cap_uF, vmax, imax, power_max, cap_name)
        self._capacitors = capacitors
    
    def xc(self, f):
        return sum([cap.xc(f) for cap in self._capacitors])
    
    def voltage(self, f, current):
        sum_voltages = sum([cap.voltage(f, current) for cap in self._capacitors])
        return sum_voltages

    def allowed_current(self, f):
        return min([cap.allowed_current(f) for cap in self._capacitors])
    

class CapacitorMaxCurrentViolationDecoratorBase(CapacitorInterface):
    def __init__(self, cap):
        self.cap = cap
    
    def xc(self, f):
        return self.cap.xc(f)
    
    def voltage(self, f, current):
        return self.cap.voltage(f, current)
    
    def name(self):
        return self.cap.name()
    
    def spec(self):
        return self.cap.spec()


class CapacitorMaxCurrentViolationDecorator(CapacitorMaxCurrentViolationDecoratorBase):
    def current(self, f, voltage):
        spec_max_current = self.spec().get_i_max()
        max_current = self.cap.current(f, voltage)
        if max_current > spec_max_current:
            raise ValueError(f"Current {max_current} exceeds maximum current {spec_max_current} of {self.name()}")
        return max_current
    

# Decorator pattern for ParallelCapacitor validating the maximum current
class ParallelCapacitorMaxCurrentViolationDecorator(CapacitorMaxCurrentViolationDecorator):
    def current(self, f, voltage):
        spec_max_current = self.spec().get_i_max()
        max_current = super().current(f, voltage)
        if max_current > spec_max_current:
            raise ValueError(f"Current {max_current} exceeds maximum current {spec_max_current} of {self.name()}")
        return max_current


# ##############################################################################
# Test cases
# ##############################################################################
import unittest


class TestCapacitor(unittest.TestCase):
    def test_capacitor(self):
        f = 1
        cap = Capacitor(cap_uF=1, vmax=1000, imax=500)
        self.assertAlmostEqual(cap.current(f, 1000), 0.006283, places=4)
        
    def test_capacitor_xc(self):
        f = 1
        cap = Capacitor(cap_uF=1, vmax=1000, imax=500)
        self.assertAlmostEqual(cap.xc(f), 159154.943, places=2)
        

    def test_capacitor_name_empty(self):
        f = 1
        cap = Capacitor(cap_uF=1, vmax=1000, imax=500)
        self.assertEqual(cap.name(), "1uF/1000V")
    
    def test_capacitor_name(self):
        f = 1
        cap = Capacitor(cap_uF=1, vmax=1000, imax=500, cap_name="C1")
        self.assertEqual(cap.name(), "C1")
    
    def test_parallel_capacitor_spec_vmax(self):
        f = 1
        cap1 = Capacitor(cap_uF=1, vmax=1000, imax=500)
        cap2 = Capacitor(cap_uF=1, vmax=100, imax=500)
        parallel = ParallelCapacitor([cap1, cap2])
        self.assertAlmostEqual(parallel.spec().get_v_max(), 100, places=0)
    
    def test_parallel_capacitor_xc(self):
        f = 1
        cap1 = Capacitor(cap_uF=1, vmax=1000, imax=500)
        cap2 = Capacitor(cap_uF=1, vmax=1000, imax=500)
        parallel = ParallelCapacitor([cap1, cap2])
        self.assertAlmostEqual(parallel.xc(f), 79577.472, places=2)
    
    def test_parallel_capacitor_current(self):
        f = 1
        cap1 = Capacitor(cap_uF=1, vmax=1000, imax=500)
        cap2 = Capacitor(cap_uF=1, vmax=1000, imax=500)
        parallel = ParallelCapacitor([cap1, cap2])
        self.assertAlmostEqual(parallel.current(f, 1000), 0.012566, places=4)

    def test_parallel_capacitor_voltage(self):
        f = 1
        cap1 = Capacitor(cap_uF=1, vmax=1000, imax=500)
        cap2 = Capacitor(cap_uF=1, vmax=1000, imax=500)
        parallel = ParallelCapacitor([cap1, cap2])
        self.assertAlmostEqual(parallel.voltage(f, 0.012566), 1000, places=1)
        
    def test_series_capacitor_xc(self):
        f = 1
        cap1 = Capacitor(cap_uF=1, vmax=1000, imax=500)
        cap2 = Capacitor(cap_uF=1, vmax=1000, imax=500)
        series = SeriesCapacitor([cap1, cap2])
        self.assertAlmostEqual(series.xc(f), 318309.886, places=2)
    
    def test_series_capacitor_current(self):
        f = 1
        cap1 = Capacitor(cap_uF=1, vmax=1000, imax=500)
        cap2 = Capacitor(cap_uF=1, vmax=1000, imax=500)
        series = SeriesCapacitor([cap1, cap2])
        self.assertAlmostEqual(series.current(f, 1000), 0.0031415926554037347, places=4)
    
    def test_series_capacitor_voltage(self):
        f = 1
        cap1 = Capacitor(cap_uF=1, vmax=1000, imax=500)
        cap2 = Capacitor(cap_uF=1, vmax=1000, imax=500)
        series = SeriesCapacitor([cap1, cap2])
        self.assertAlmostEqual(series.voltage(f, 0.0031415926554037347), 1000, places=3)

    def test_parallel_capacitor_decorator_current(self):
        f = 1
        cap1 = Capacitor(cap_uF=1, vmax=1000, imax=500)
        cap2 = Capacitor(cap_uF=1, vmax=1000, imax=500)
        parallel = ParallelCapacitorMaxCurrentViolationDecorator(ParallelCapacitor([cap1, cap2]))
        self.assertAlmostEqual(parallel.current(f, 1000), 0.012566, places=4)
    
    def test_parallel_capacitor_decorator_current_exceeding_range(self):
        f = 50000
        cap1 = Capacitor(cap_uF=10, vmax=1000, imax=500)
        cap2 = Capacitor(cap_uF=20, vmax=800, imax=600)
        parallel = ParallelCapacitorMaxCurrentViolationDecorator(ParallelCapacitor([cap1, cap2]))
        with self.assertRaises(ValueError) as context:
            parallel.current(f, 1000)
        self.assertEqual(context.exception.args[0], "Current 9424.777960769377 exceeds maximum current 1100 of parallel group 30uF/800V")
    
    def test_capacitor_decorator_exceeding_range(self):
        f = 50000
        cap = Capacitor(cap_uF=10, vmax=1000, imax=500)
        cap = CapacitorMaxCurrentViolationDecorator(cap)
        with self.assertRaises(ValueError) as context:
            cap.current(f, 1000)
        self.assertEqual(context.exception.args[0], "Current 3141.5926535897925 exceeds maximum current 500 of 10uF/1000V")    
    
    def test_parallel_group_single_capacitor_exceeding_range(self):
        f = 50000
        cap1 = CapacitorMaxCurrentViolationDecorator(Capacitor(cap_uF=10, vmax=1000, imax=500))
        cap2 = CapacitorMaxCurrentViolationDecorator(Capacitor(cap_uF=20, vmax=800, imax=600))
        parallel = ParallelCapacitor([cap1, cap2])
        with self.assertRaises(ValueError) as context:
            parallel.current(f, 1000)
        self.assertEqual(context.exception.args[0], "Current 3141.5926535897925 exceeds maximum current 500 of 10uF/1000V")
    
    def test_series_capacitor(self):
        f = 1
        cap1 = Capacitor(cap_uF=1, vmax=1000, imax=500)
        cap2 = Capacitor(cap_uF=1, vmax=1000, imax=500)
        series = SeriesCapacitor([cap1, cap2])
        
        xc = 1/(2 * math.pi * f * 0.5e-6)
        expected_current = 1000/xc
        self.assertAlmostEqual(series.current(f, 1000), expected_current, places=4)

        cap1 = Capacitor(cap_uF=1, vmax=1000, imax=500)
        cap2 = Capacitor(cap_uF=3, vmax=1000, imax=500)
        series = SeriesCapacitor([cap1, cap2])
        
        xc = 1/(2 * math.pi * f * 0.75e-6)
        expected_current = 1000/xc
        self.assertAlmostEqual(series.current(f, 1000), expected_current, places=4)
    
    def test_series_capacitor_voltage(self):
        f = 1
        cap1 = Capacitor(cap_uF=1, vmax=1000, imax=500)
        cap2 = Capacitor(cap_uF=1, vmax=1000, imax=500)
        series = SeriesCapacitor([cap1, cap2])

        xc = 1/(2 * math.pi * f * 0.5e-6)
        expected_voltage = 0.0031415926554037347 * xc
        self.assertAlmostEqual(series.voltage(f, 0.0031415926554037347), expected_voltage, places=3)

    def test_parallel_capacitors_allowed_max_current_decorator(self):
        f = 1
        cap1 = Capacitor(cap_uF=10, vmax=1000, imax=500)
        cap2 = Capacitor(cap_uF=20, vmax=800, imax=600)
        parallel = ParallelCapacitor([cap1, cap2])

        c = 10 + 20
        xc = 1/(2 * math.pi * f * c*1e-6)
        expected_current = 800/xc

        self.assertAlmostEqual(parallel.allowed_current(f), expected_current, places=0)

    def test_series_capacitors_allowed_max_current_decorator(self):
        f = 1
        cap1 = Capacitor(cap_uF=10, vmax=1000, imax=500)
        cap2 = Capacitor(cap_uF=20, vmax=800, imax=600)
        series = SeriesCapacitor([cap1, cap2])

        c = 20
        xc = 1/(2 * math.pi * f * c*1e-6)
        expected_current = 800/xc

        self.assertAlmostEqual(series.allowed_current(f), expected_current, places=0)
        
        
unittest.main(argv=[''], exit=False)
