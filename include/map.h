#pragma once
#include <lanelet2_core/primitives/Lanelet.h>

namespace cat
{
class HDMap
{
private:
    lanelet::LaneletMapPtr map;
public:
    HDMap(/* args */) = default;
    ~HDMap() = default;

    void init();
    std::string generateTempFileName(const std::string& name);
};

}  // namespace cat