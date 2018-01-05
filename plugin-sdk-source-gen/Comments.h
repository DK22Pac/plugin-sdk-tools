#pragma once
#include <string>
#include <fstream>
#include "Tabs.h"

using namespace std;

void WriteComment(ofstream &stream, string const &comment, tabs t, unsigned int pos);
