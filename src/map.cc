#include <lanelet2_core/primitives/Lanelet.h>
#include <lanelet2_io/Io.h>
#include <lanelet2_io/io_handlers/Factory.h>
#include <lanelet2_io/io_handlers/Writer.h>
#include <lanelet2_projection/UTM.h>
#include <map.h>
#include <cstdio>

namespace
{
std::string exampleMapPath = "../third_party/Lanelet2/lanelet2_maps/res/mapping_example.osm";

std::string tempfile(const std::string& name)
{
    char tmpDir[] = "/tmp/lanelet2_example_XXXXXX";
    auto* file = mkdtemp(tmpDir);
    if (file == nullptr)
    {
        throw lanelet::IOError("Failed to open a temporary file for writing");
    }
    return std::string(file) + '/' + name;
}
}  // namespace

void part1LoadingAndWriting()
{
    using namespace lanelet;
    // loading a map requires two things: the path and either an origin or a projector that does the
    // lat/lon->x/y conversion.
    projection::UtmProjector projector(Origin({49, 8.4}));  // we will go into details later
    LaneletMapPtr map = load(exampleMapPath, projector);

    // the load and write functions are agnostic to the file extension. Depending on the extension,
    // a different loading algorithm will be chosen. Here we chose osm.

    // we can also load and write into an internal binary format. It is not human readable but
    // loading is much faster than from .osm:
    write(tempfile("map.bin"), *map);  // we do not need a projector to write to bin

    // if the map could not be parsed, exceptoins are thrown. Alternatively, you can provide an
    // error struct. Then lanelet2 will load the map as far as possible and write all the errors
    // that occured to the error object that you passed:
    ErrorMessages errors;
    map = load(exampleMapPath, projector, &errors);
    assert(errors.empty());  // of no errors occurred, the map could be fully parsed.
}

int main()
{
    // this tutorial shows you how to load and write lanelet maps. It is divided into three parts:
    part1LoadingAndWriting();
    return 0;
}
