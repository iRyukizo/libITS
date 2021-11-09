#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

class Aggregation
{
public:
    Aggregation(const std::string &filename);

    std::map<std::string, int> process();
    unsigned long arrangement(int value, const std::string &name) const;

private:
    std::string filename_;
    std::map<std::string, int> aggregate_;
};
