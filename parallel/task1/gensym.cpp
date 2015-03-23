#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

int main(int argc, char **argv) {
  if (argc != 4) {
    cerr << "Usage: " << argv[0] << " size mat_file vec_file" << endl;
    return 1;
  }

  int size = atoi(argv[1]);
  ofstream out;
/*  ofstream out;
  out.open(argv[2]);
  out << size << endl;
  for (int i = 0; i < size; ++i) {
    for (int j = 0; j < size; ++j)
      out << (i == j ? "2 " : "1 ");
    out << endl;
  }
  out.close();*/

  out.open(argv[3]);
  out << size << endl;
  for (int i  = 0 ; i < size; ++i) 
    out << (size + 1) << endl;
  out.close();

  return 0;
}
