#pragma once
#include <string>
#include <fstream>
#include "Tabs.h"
#include "..\shared\Games.h"

using namespace std;

void WriteComment(ofstream &stream, string const &comment, tabs t, unsigned int pos);
std::string GetPluginSdkComment(Games::IDs game, bool isHeader);
