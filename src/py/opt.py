import hf.hfopt_lib

def version():
  return hf.hfopt_lib.version() 

def module_names():
  return hf.hfopt_lib.get_module_names()

class Design:
  def __init__(self):
    self.min_range =0
    self.min_twr   =1
    self.min_spd   =0
    self.modules   =[]
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

  def fill_constraints():
    """
    This is the main method, which calls out to the C libraries to do
    optimization / fill-in of the missing modules.  If you have not specified
    any required modules (set_req) or constraints (set_min_X), then it will
    return a trivial empty solution.
    """
    pass
