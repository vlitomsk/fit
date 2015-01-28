#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <tclap/CmdLine.h>
#include "PlanarSurface.h"
#include "TorusSurface.h"
#include "CylinderSurface.h"
#include "Robot.h"

using namespace std;

int main(int argc, char **argv) {
  try {
    TCLAP::CmdLine cmd("Command description message", ' ', "0.33");
    TCLAP::ValueArg<std::string> topologyArg("t", "topology", "Topology (planar, tor, cylinder)", false, "planar", "string");
    TCLAP::ValueArg<std::string> spaceArg("s", "space", "Input surface file. Default space.txt", false, "space.txt", "string");
    TCLAP::ValueArg<std::string> outArg("o", "out", "Output surface file. Default route.txt", false, "route.txt", "string");
    TCLAP::ValueArg<uint> limitArg("l", "limit", "Maximum path length", false, 1000, "integer");
    cmd.add(topologyArg);
    cmd.add(spaceArg);
    cmd.add(outArg);
    cmd.add(limitArg);
    cmd.parse(argc, argv);

    Surface *surf;
    string topology = topologyArg.getValue();
    if (topology == "planar") {
      surf = new PlanarSurface();
    } else if (topology == "tor") {
      surf = new TorusSurface();
    } else if (topology == "cylinder") {
      surf = new CylinderSurface();
    } else
      throw TCLAP::ArgException("This topology is not supported!", "topology");
  
    cout << "Using input: " << spaceArg.getValue() << endl;
    cout << "Using output: " << outArg.getValue() << endl;
    ifstream spaceStream(spaceArg.getValue());
    ofstream outStream(outArg.getValue());
    spaceStream >> (*surf);

    Robot robot(*surf);
    robot.run();

    robot.printResults(outStream);
    robot.printResults(cout);

    delete surf;
  } catch (TCLAP::ArgException & e) {
    cerr << "Error: " << e.error() << " for arg " << e.argId() << endl;
  }

  return EXIT_SUCCESS;
}
