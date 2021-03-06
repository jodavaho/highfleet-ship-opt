# Highfleet ship optimizer

Or, a cogitator for your dieselpunk legos.

Highfleet ([koshutin.com](http://koshutin.com/)) is a game by Konstantin Koshutin, of Hammerflight fame. In that game
you have to bulid, peice by peice, huge rocket-propelled fleets to fight above
a primitive desert society. 
The attention to detail is immaculate. Radio communications, electronic
intelligence / warfare, and strike fleet management are just the tip of the
iceburg. It's great fun, for engineers especially. 

What gets most people addicted, however, is the ship editing. You can create
and build ships from a suprisingly few number of modules that become highly
individualized / specialized. This gif (from [koshutin.com](http://koshutin.com/)) shows the design loop perfectly:

![Highfleet ship design from koshutin.com](http://koshutin.com/wp-content/uploads/2018/05/gif_design.gif)

But seriously, check out that page, the game is beautiful.
The most challenging part of building a *good* ship, is keeping the number of
extra modules low, and selecting the appropriate modules to keep the cost low
and performance high. 

This code will help you. It looks like this:

![image](https://user-images.githubusercontent.com/6580327/153905788-a8307fe4-17a4-469a-96b2-462e5448f550.png)

This codebase selects modules from the game Highfleet, such that desired
in-game statistics are satisfied. It does so optimally and fast, using
state-of-the-art solver library SCIP.

This library contains 3 things:

1. The solver wrapper (`make lib`)
2. A CLI application (`make hfopt`)
3. Python wrappers (`pip install .`)

This project is also hosted on [jodavaho.io/hf/opt](https://jodavaho.io/hf/opt)
if you'd like to give it a shot (or will be shortly!)

Most use cases are one of the following:

- I have a number of weapons and sensor that I'd like to include, but don't know the best set of engines and support modules to optimally include them
- I have a scout ship in mind, one that can go a certain speed and travel a certain range with a few sensors. 
- I'm curious what is the cheapest ship that can accomodate a given weapon or sensor, given a speed, weight, range, or other constraint

As always, please feel free to file issues here with questions, suggestions, or bugs. 

**I especially need help verifying the stats of the output modules, once combined**. The math behind the game is notoriously opaque, and all the stats used were compiled from [reddit.com/r/highfleet](https://reddit.com/r/highfleet), esp `/u/d0d0b1rd` and `/u/twiglard`. 

# Installation

1. Install SCIP: [https://www.scipopt.org/download.php](https://www.scipopt.org/download.php)
2. Get the .whl: `wget https://github.com/jodavaho/highfleet-ship-opt/raw/public-main/releases/hfopt-1.0.0-cp38-cp38-linux_x86_64.whl` and install it `pip install hfopt-1.0.0-cp38-cp38-linux_x86_64.whl`

# Example

The simplest example is as follows, which is pretty much as previous example:

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

# Building 

if you'd like to build the cli app, C/C++ library, or python library, we require a few things.

## Requires: 

- SCIP: `https://www.scipopt.org/download.php?fname=SCIPOptSuite-8.0.0-Linux-ubuntu.deb`
  - installed like this: `sudo apt install ./SCIPOptSuite-8.0.0-Linux-ubuntu.deb  --fix-broken`

## OR, compile your own custom SCIP (from source)

If you wish to build this repo and SCIP from source, we just need this: ` make scipoptlib SHARED=true`

## hfopt (main executable)

- Nothing extra, just `make hfopt`

## For windows 

Give it a shot using `pip install .` or the whl above, and let me know if it fails.

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
1. Add extra framing squares ( simply add constant to constraint )
1. .deb installers
1. https://github.com/pypa/manylinux
1. PyPI
