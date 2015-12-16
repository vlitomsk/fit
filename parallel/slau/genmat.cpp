#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

int main(int argc, char **argv) {
  if (argc != 3) {
    cerr << "Usage: " << argv[0] << " outFile sz" << endl;
  }
  ofstream out(argv[1]);
  int sz = atoi(argv[2]);
  out << sz << endl;
  for (int i = 0; i < sz; ++i)
    out << (sz + 1) << " ";
  out << endl;
  for (int i = 0; i < sz; ++i) {
    for (int j = 0; j < sz; ++j) 
      out << (i == j ? "2" : "1") << " ";
    out << endl;
  }
  out.close();
  return 0;
}
