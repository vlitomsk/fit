#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <fstream>

using namespace std;

int main(int argc, char **argv) {
  if (argc != 4) {
    cerr << "Usage: " << argv[0] << " size mat_file vec_file" << endl;
    return 1;
  }

  int size = atoi(argv[1]);
  uint8_t *arr = new (std::nothrow) uint8_t[size * (size + 1) / 2];
  if (!arr) {
    cerr << "Can't alloc memory" << endl;
    return 1;
  }
  for (int i = 0; i < size * (size + 1) / 2; ++i) {
    arr[i] = rand() % 10;
  }

  ofstream out(argv[2]);
  out << size << endl;
  for (int j = 0; j < size; ++j) {
    for (int i = 0; i < size; ++i) {
      if (i <= j)
        out << (int)arr[j * (j + 1) / 2 + i] << ' ';
      else 
        out << (int)arr[i * (i + 1) / 2 + j] << ' ';
    }
    out << endl;
  }
  out.close();

  out.open(argv[3]);
  int x = rand() % 10;
  out << size << endl;
  for (int i = 0; i < size; ++i)
    out << x << endl;
  out.close();
  
  delete[] arr;

  return 0;
}
