# Build-Depends

- SCIP is at: `https://www.scipopt.org/download.php?fname=SCIPOptSuite-8.0.0-Linux-ubuntu.deb`
- installed like this: `sudo apt install ./SCIPOptSuite-8.0.0-Linux-ubuntu.deb  --fix-broken`

# Scip binaries depend on:

- libgmp-dev
- libreadline-dev

So an easy build includes those. You can use buildflags to disable those, however.

we just need this: ` make scipoptlib SHARED=true`

- We then use:

```
cp scip/lib/shared/libscip.so $(DEPS)
cp scip/lib/static/libscip.a $(DEPS)
```

with `$(DEPS)=.deps/x86_64-linux-gnu/lib/`, with make install INSTALLDIR=.deps/$(TARGET) or something

# For python wrappers (make python)

- libpython3.8-dev

# For windows (make windows)

- g++-mingw*

