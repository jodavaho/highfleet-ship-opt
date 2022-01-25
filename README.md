# Build-Depends

- SCIP is at: `https://www.scipopt.org/download.php?fname=SCIPOptSuite-8.0.0-Linux-ubuntu.deb`
- installed like this: `sudo apt install ./SCIPOptSuite-8.0.0-Linux-ubuntu.deb  --fix-broken`

# Scip binaries depend on:

- libgmp-dev
- libreadline-dev

So an easy build includes those. You can use buildflags to disable those, however.

- We then use:
  - `scip/lib/static/libscipbase-8.0.0.linux.x86_64.gnu.opt.a`
  - `scip/lib/static/liblpispx2-8.0.0.linux.x86_64.gnu.opt.a`
  - `/lib/x86_64-linux-gnu/libgmp.a`
  - `/lib/x86_64-linux-gnu/libreadline.a`

in `.deps/x86_64-linux-gnu/lib/`, with make install INSTALLDIR=.deps/$(TARGET) or something

# For python wrappers (make python)

- libpython3.8-dev

# For windows (make windows)

- g++-mingw*

