#ifndef __UTILS_H__
#define __UTILS_H__

#include <gecode/int.hh>
#include <gecode/minimodel.hh>
#include <gecode/search.hh>
#include <gecode/driver.hh>

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono> 

using namespace std;
using namespace std::chrono; 
using namespace Gecode;
using namespace std;

// parameters
string INPUT_FILE="./data/cos_50_5_2";
string OUTPUT_FILE="";
bool INTERMEDIATE = false;

template<class T>
inline void fromString(const string &s, T &x)
{
    stringstream in(s);
    in >> x;
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

string format_duration(microseconds ms ) {
    using namespace std::chrono;
    auto secs = duration_cast<seconds>(ms);
    ms -= duration_cast<microseconds>(secs);
    auto mins = duration_cast<minutes>(secs);
    secs -= duration_cast<seconds>(mins);
    auto hour = duration_cast<hours>(mins);
    mins -= duration_cast<minutes>(hour);

    stringstream ss;
    if (hour.count() != 0)
        ss << hour.count() << ":";
    if (mins.count() != 0)
        ss << mins.count() << ":" ;
    if (secs.count() != 0) 
        ss << secs.count() << "." ;
    ss << float(ms.count());
    return ss.str();
}

#endif