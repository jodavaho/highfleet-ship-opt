#include "read-problem.hpp"
#include "hf-problem.hpp"

void read_from(
    std::istream &is,
    Bounds &out_bounds,
    std::vector<module> &out_required,
    SolveOptions &out_opts
    )
{

}
void read_from_stdin(
    Bounds &out_bounds,
    std::vector<module> &out_required,
    SolveOptions &out_opts
    )
{
  read_from(
      std::cin,
      out_bounds,
      out_required,
      out_opts
      );
}

