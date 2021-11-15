#include "aggregation.hh"

Aggregation::Aggregation(const std::string &filename)
    : filename_(filename)
{}

std::unordered_map<std::string, int> Aggregation::process()
{
    std::ifstream file(filename_);
    if (!file.is_open())
        throw std::ios_base::failure("Couln't open file " + filename_);

    std::unordered_map<std::string, int> map;
    std::string line;
    int nb_lines = 0;
    while (!std::getline(file, line).eof())
    {
        std::stringstream line_stream(line);
        std::string v_name;
        std::string v_value;
        std::string dump;
        nb_lines++;
        if (!(line_stream >> v_name) || !(line_stream >> v_value)
            || (line_stream >> dump))
            throw std::ios_base::failure("Number of values incorrect at line "
                                         + std::to_string(nb_lines)
                                         + " of file " + filename_);
        if (v_name[0] == '#')
            continue;
        int v_val;
        try
        {
            v_val = std::stoi(v_value);
        }
        catch (const std::exception &e)
        {
            throw std::ios_base::failure("Couldn't convert values at line "
                                         + std::to_string(nb_lines)
                                         + " of file " + filename_);
        }
        map[v_name] = v_val;
    }

    aggregate_ = map;

    return map;
}

unsigned long Aggregation::arrangement(int value, const std::string &name) const
{
    unsigned long repr = value + aggregate_.at(name) - 1;
    std::string bitmask(value, 1);
    bitmask.resize(repr, 0);
    auto count = 1;
    while (std::prev_permutation(bitmask.begin(), bitmask.end()))
        count++;
    return count;
}
