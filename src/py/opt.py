import hf.hfopt_lib


def version():
  return hf.hfopt_lib.version() 

def module_names():
  return hf.hfopt_lib.get_module_names()

def example():
  d= Design()
  print("Create something like a Lightning")
  # Set a module requirement for 2 ak100s
  d.modules["gun_ak100"]=2
  # and minimum range:
  d.min_range=800
  # and make if faster
  d.min_spd=600
  d.dump_modules()
  d.dump_constraints()
  d.fill_constraints()
  # Now we have a filled module list
  print("Post-solve:")
  d.dump_modules()

class Design:

  def __init__(self):
    self.min_range   =0
    self.min_twr     =1
    self.min_spd     =0
    self.modules     ={x:0  for x in module_names()}
    pass

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
