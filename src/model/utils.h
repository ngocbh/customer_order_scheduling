#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono> 
#include <string>
#include <sstream>
#include <assert.h>

using namespace std;
using namespace std::chrono; 

// parameters
string INPUT_FILE="";
string OUTPUT_FILE="";
bool INTERMEDIATE = false;

template<class T>
inline void fromString(const string &s, T &x)
{
    stringstream inp(s);
    inp >> x;
}

void parseCommandFlags(int argc, char* argv[])
{
    for (int i = 1; i < argc; ++ i) {
        if (!strcmp(argv[i], "--input") || !strcmp(argv[i], "-i") ) {
            fromString(argv[++ i], INPUT_FILE);
        } else if (!strcmp(argv[i], "--output") || !strcmp(argv[i], "-o") ) {
            fromString(argv[++ i], OUTPUT_FILE);
        } else if (!strcmp(argv[i], "--verbose")) {
            INTERMEDIATE = true;
        } else {
            fprintf(stderr, "[Warning] Unknown Parameter: %s\n", argv[i]);
        }
    }
}

inline bool my_assert(bool flg, string msg)
{
    if (!flg) {
        cerr << msg << endl;
        // exit(-1);
    }
    return flg;
}

string format_duration(microseconds ms ) {
    using namespace std::chrono;
    auto secs = duration_cast<seconds>(ms);
    // ms -= duration_cast<microseconds>(secs);
    auto mins = duration_cast<minutes>(secs);
    secs -= duration_cast<seconds>(mins);
    auto hour = duration_cast<hours>(mins);
    mins -= duration_cast<minutes>(hour);

    stringstream ss;
    if (hour.count() != 0)
        ss << hour.count() << ":";
    if (mins.count() != 0)
        ss << mins.count() << ":" ;
    ss << float(ms.count())/10e5;
    return ss.str();
}

#endif