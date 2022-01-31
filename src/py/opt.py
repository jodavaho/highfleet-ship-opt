import hf.hfopt_lib

def version():
  return hf.hfopt_lib.version() 

def module_names():
  return hf.hfopt_lib.get_module_names()

def example():
  d= Design()
  print("How many included by default?")
  print(d.modules)
  # Set a module requirement for 2 ak100s
  d.modules["gun_ak100"]=2
  # and minimum range:
  d.min_range=1000
  # and make if faster
  d.min_spd = 350
  d.fill_constraints()
  # Now we have a filled module list
  print(d.modules)

class Design:
  def __init__(self):
    self.min_range   =0
    self.min_twr     =1
    self.min_spd     =0
    self.modules     ={x:0  for x in module_names()}
    pass

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
    pass
