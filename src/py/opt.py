import hf.hfopt_lib

def version():
  return hf.hfopt_lib.version() 

def is_module(name):
  return hf.hfopt_lib.is_module(name)

def module_names():
  return hf.hfopt_lib.get_module_names()

def example():
  d= Design()
  print("Create something like a Lightning")
  # Set a module requirement for 2 ak100s and some safety equipment
  d.require("100mm",2)
  d.require("pod",2)
  d.require("fss",2)
  # and landing gear b/c we're not a savage
  d.require("chassis_m",4)
  # and minimum range:
  d.set_min_range(800)
  # and make it fast
  d.set_min_spd(650)
  d.dump_modules()
  d.dump_constraints()
  d.fill_constraints()
  # Now we have a filled module list
  print("============================")
  print("Here's the list of modules: ")
  d.dump_modules()


class Module:

  def __init__(self):
    self.cost=0
    self.name=""
    self.energy=0
    self.ammo=0
    self.crew=0
    self.weight=0
    self.thrust=0

  def __init__(self,name):
    attrdict = hf.hfopt_lib.module_stats(name)
    if (attrdict is None):
        return
    self.cost=attrdict["cost"]
    self.name=attrdict["name"]
    self.energy=attrdict["energy"]
    self.ammo=attrdict["ammo"]
    self.crew=attrdict["crew"]
    self.weight=attrdict["weight"]
    self.thrust=attrdict["thrust"]

class Design:

  def __init__(self):
    self.min_range   =0
    self.min_twr     =1
    self.min_spd     =0
    self.modules     ={}

  def require(self, name, count):
    if is_module(name):
      self.modules[name]=count

  def dump_modules(self):
    for mc in self.modules.keys():
        if self.modules[mc] > 0:
            print("{}={}".format(mc,self.modules[mc]))

  def dump_constraints(self):
      print('min_range={}'.format(self.min_range))
      print('min_spd={}'.format(self.min_spd))
      print('min_twr={}'.format(self.min_twr))

  def set_min_range(self,r):
    self.min_range = r

  def set_min_spd(self,s):
    """
    This will add the constraint that speed > s, which will impact min_twr.
    The solver will sort it out.
    """
    self.min_spd = s

  def set_min_twr(self,t):
    """
    This will add the constraint that T/W > T, which may also affect min_spd
    The solver will sort it out.
    """
    self.min_twr = t

  def fill_constraints(self):
    """
    This is the main method, which calls out to the C libraries to do
    optimization / fill-in of the missing modules.  If you have not specified
    any required modules (set_req) or constraints (set_min_X), then it will
    return a trivial empty solution.
    """
    self.modules = hf.hfopt_lib.solve_fill(
            self.modules, 
            self.min_range,
            self.min_spd,
            self.min_twr)
