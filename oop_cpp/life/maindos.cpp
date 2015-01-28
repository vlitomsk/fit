#include "GraphCtl.h"
#include <string>
#include <iostream>
#include <cstring>

using namespace std;

char shortArg_iter[] = "-i";
char shortArg_output[] = "-o";
char longArg_iter[] = "--iterations=";
char longArg_output[] = "--output=";

char defaultInput[] = "dflinput.lif";

size_t parseIters(int argc, char const *argv[]) {
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], shortArg_iter)) {
            if (i == argc - 1) {
                cerr << "Bad args\n";
                return 1; // default value iters = 1
            } else 
                return atoi(argv[i + 1]);
        }

        if (!strncmp(argv[i], longArg_iter, strlen(longArg_iter))) {
            return atoi(argv[i] + strlen(longArg_iter));
        }
    }

    return 1;
}

const char* parseOutFile(int argc, char const *argv[]) {
    for (size_t i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "-o")) {
            if (i == argc - 1) {
                cerr << "Bad args\n";
                return NULL;
            } else {
                return argv[i + 1];
            }
        }

        if (!strncmp(argv[i], "--output=", strlen("--output="))) {
            return argv[i] + strlen(longArg_output);
        }
    }

    return NULL;
}

const char* parseInFile(int argc, char const *argv[]) {
    if (argc >= 2) {
        if (argv[1][0] != '-') 
            return argv[1];
    }
    return defaultInput;
}

int main(int argc, char const *argv[])
{
    /* 
      Usage: 
         ./program [inputFile] [-i iters] [--iterations=iters] [-o output] [--output=output]
    */

    const char *inFile = parseInFile(argc, argv);
    const char *outFile = parseOutFile(argc, argv);
    size_t iters = parseIters(argc, argv);
    bool offlineMode = outFile != NULL;

    //offlineMode = false;
    Life *life = new Life(inFile);
    if (offlineMode) {
        cout << "Using offline mode" << endl;
        life->step(iters);
        life->dump(outFile);
    } else {
        cout << "Using online mode" << endl;
        GraphCtl ctl(*life);
        ctl.start();
    }
    delete life;

    return 0;
}