# Highfleet ship optimizer

Or, a cogitator for your dieselpunk legos.

Highfleet is a game by Konstantin Koshutin, of Hammerflight fame. In that game
you have to bulid, peice by peice, huge rocket-propelled fleets to fight above
a primitive desert society. 
The attention to detail is immaculate. Radio communications, electronic
intelligence / warfare, and strike fleet management are just the tip of the
iceburg. It's great fun, for engineers especially. 

What gets most people addicted, however, is the ship editing. You can create
and build ships from a suprisingly few number of modules that become highly
individualized / specialized. 

The most challenging part of building a *good* ship, is keeping the number of
extra modules low, and selecting the appropriate modules to keep the cost low
and performance high. 

This code will help you.

This codebase selects modules from the game Highfleet, such that desired
in-game statistics are satisfied. It does so optimally and fast, using
state-of-the-art solver library SCIP.

This library contains 3 things:

1. The solver wrapper (`make lib`)
2. A CLI application (`make hfopt`)
3. Python wrappers (`pip install .`)

This project is also hosted on [jodavaho.io/hf/opt](https://jodavaho.io/hf/opt)
if you'd like to give it a shot (sorry about the primitive interface!)

Most use cases are one of the following:

- I have a number of weapons and sensor that I'd like to include, but don't know the best set of engines and support modules to optimally include them
- I have a scout ship in mind, one that can go a certain speed and travel a certain range with a few sensors. 
- I'm curious what is the cheapest ship that can accomodate a given weapon or sensor, given a speed, weight, range, or other constraint

As always, please feel free to file issues here with questions, suggestions, or bugs. 

# Example

The simplest example is as follows:

```python
import hf.opt as o

d= Design()
# Set a module requirement for 2 ak100s and some safety equipment
d.require("100mm",2)
d.require("pod",2) #escape pods
d.require("fss",2) #fire supression system
# and landing gear b/c we're not a savage
d.require("chassis_m",4)
# and minimum range:
d.set_min_range(800)
# and make it fast
d.set_min_spd(650)
d.dump_modules() # check included modules
d.dump_constraints() # and constraints
d.fill_constraints() # invoke solver to fill in missing mods
# Now we have a filled module list
print("============================")
print("Here's the list of modules: ")
d.dump_modules()
```

# Build-Depends

if you'd like to build the cli app, C/C++ library, or python library, we require a few things.

- SCIP: `https://www.scipopt.org/download.php?fname=SCIPOptSuite-8.0.0-Linux-ubuntu.deb`
  - installed like this: `sudo apt install ./SCIPOptSuite-8.0.0-Linux-ubuntu.deb  --fix-broken`

## hfopt (main executable)

- Nothing extra

## custom SCIP (from source)

If you wish to build this repo and SCIP from source, we just need this: ` make scipoptlib SHARED=true`

Then:

```
cp scip/lib/shared/libscip.so $(DEPS)
cp scip/lib/static/libscip.a $(DEPS)
```

with `$(DEPS)=.deps/x86_64-linux-gnu/lib/`.

## For windows 

Give it a shot using `pip install .` and let me know if it fails.

## Python libraries / bindings

`make python`

And see output in:

`build/lib.*`

Or just

`pip install .`

# License

Since we do not copy, modify, or redistribute SCIP, all code in this repo is under the MIT License. See LICENSE.

# Todos

1. Add minimum fuel to allow tanker optmizations
2. Add extra framing squares ( simply add constant to constraint )
3. set up buildchain for windows 
  - (https://flask.palletsprojects.com/en/2.0.x/tutorial/install/ )
  - https://packaging.python.org/en/latest/tutorials/packaging-projects/
4. Public github
5. .deb installers
6. PyPI
7. 
