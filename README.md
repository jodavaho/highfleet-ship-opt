# Build-Depends

- SCIP is at: `https://www.scipopt.org/download.php?fname=SCIPOptSuite-8.0.0-Linux-ubuntu.deb`
- installed like this: `sudo apt install ./SCIPOptSuite-8.0.0-Linux-ubuntu.deb  --fix-broken`

# Scip static depends:

- libgmp-dev
- libreadline-dev
- We then use:
  - `scip/lib/static/libscipbase-8.0.0.linux.x86_64.gnu.opt.a`
  - `scip/lib/static/liblpispx2-8.0.0.linux.x86_64.gnu.opt.a`
  - `/lib/x86_64-linux-gnu/libgmp.a`
  - `/lib/x86_64-linux-gnu/libreadline.a`

# For python wrappers (make python)

- libpython3.8-dev


# For windows (make windows)

- g++-mingw*

