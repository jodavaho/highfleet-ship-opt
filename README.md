#Build-Depends

## hfopt (main executable)

- SCIP: `https://www.scipopt.org/download.php?fname=SCIPOptSuite-8.0.0-Linux-ubuntu.deb`
  - installed like this: `sudo apt install ./SCIPOptSuite-8.0.0-Linux-ubuntu.deb  --fix-broken`
  - If you wish to build this repo and SCIP from source, we just need this: ` make scipoptlib SHARED=true`
- Then:

```
cp scip/lib/shared/libscip.so $(DEPS)
cp scip/lib/static/libscip.a $(DEPS)
```
with `$(DEPS)=.deps/x86_64-linux-gnu/lib/`.

## Python wrappers

- For the python wrappers install (on ubuntu/wsl) `libpython3.8-dev`

`make python`

# For windows 


- `g++-mingw*`

`make windows`

# License
Since we do not copy, modify, or redistribute SCIP, all code in this repo is under the MIT License
