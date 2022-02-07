# Highfleet ship optimizer

This codebase selects modules from the game Highfleet, such that desired
in-game statistics are satisfied. It does so optimally and fast, using
state-of-the-art solver library SCIP.

This library contains 3 things:

1. The solver wrapper (`make lib`)
2. A CLI application (`make hfopt`)
3. Python wrappers (`make python` for a Flask deployment as a webservice)

# Build-Depends

## hfopt (main executable)

- SCIP: `https://www.scipopt.org/download.php?fname=SCIPOptSuite-8.0.0-Linux-ubuntu.deb`
  - installed like this: `sudo apt install ./SCIPOptSuite-8.0.0-Linux-ubuntu.deb  --fix-broken`

## custom SCIP (from source)

If you wish to build this repo and SCIP from source, we just need this: ` make scipoptlib SHARED=true`

Then:

```
cp scip/lib/shared/libscip.so $(DEPS)
cp scip/lib/static/libscip.a $(DEPS)
```

with `$(DEPS)=.deps/x86_64-linux-gnu/lib/`.

## Python wrappers

- For the python wrappers install (on ubuntu/wsl) `libpython3.8-dev`

`make python`

## For windows 

Not currently working. 

Why:

- `g++-mingw*`

`make windows`

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
