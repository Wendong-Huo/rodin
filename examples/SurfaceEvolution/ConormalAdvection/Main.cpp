#include <Rodin/Math.h>
#include <Rodin/Solver.h>
#include <Rodin/Geometry.h>
#include <Rodin/Variational.h>
#include <RodinExternal/MMG.h>

using namespace std;
using namespace Rodin;
using namespace Rodin::Geometry;
using namespace Rodin::Variational;
using namespace Rodin::External;

const char* meshFile =
  "../resources/examples/SurfaceEvolution/ConormalAdvection/Surface.mesh";

int main()
{
  Mesh th;
  th.load(meshFile);
  H1 vh(th);

  GridFunction dist(vh);
  MMG::Distancer(vh).setInteriorDomain(1).distance(th);

  std::ofstream fout("obj.txt");

  size_t N = 100;
  for (size_t i = 0; i < N; i++)
  {
    // fout << err << "\n";
    fout.flush();
  }

  th = MMG::ImplicitDomainMesher().split(6, {3, 6})
                                     .noSplit(2)
                    .setHMax(0.05)
                    .surface()
                    .discretize(dist);
  return 0;
}
