#pragma once
#include <lanelet2_core/primitives/Lanelet.h>
#include <lanelet2_io/Io.h>
#include <lanelet2_io/io_handlers/BinHandler.h>
#include <lanelet2_io/io_handlers/Factory.h>
#include <lanelet2_io/io_handlers/OsmHandler.h>
#include <lanelet2_io/io_handlers/Writer.h>
#include <lanelet2_projection/UTM.h>

namespace cat
{
class HDMap
{
private:

public:
    HDMap(/* args */) = default;
    ~HDMap() = default;

    void init();
    std::string generateTempFileName(const std::string& name);
    lanelet::LaneletMapPtr map;
};

}  // namespace cat