#include <map.h>
#include <cstdio>

namespace cat
{
void HDMap::init()
{
    using namespace lanelet;
    std::string exampleMapPath = "/home/uisee/desktop/ads_viz/third_party/Lanelet2/lanelet2_maps/"
                                 "res/mapping_example.osm";

    // init
    // register with factories
    io_handlers::RegisterParser<lanelet::io_handlers::OsmParser> reg_osm_parser;

    // register with factories
    io_handlers::RegisterWriter<io_handlers::OsmWriter> reg_osm_writer;

    // register with factories
    io_handlers::RegisterParser<io_handlers::BinParser> reg_bin_parser;
    io_handlers::RegisterWriter<io_handlers::BinWriter> reg_bin_writer;

    // loading a map requires two things: the path and either an origin or a projector that does the
    // lat/lon->x/y conversion.
    projection::UtmProjector projector(Origin({49, 8.4}));  // we will go into details later
    lanelet::ErrorMessages errors;
    io::Configuration params;
    map = load(exampleMapPath, projector, &errors, params);

    printf("lanelet size[%d]\n", int32_t(map->laneletLayer.size()));

    // the load and write functions are agnostic to the file extension. Depending on the extension,
    // a different loading algorithm will be chosen. Here we chose osm.
    // we can also load and write into an internal binary format. It is not human readable but
    // loading is much faster than from .osm:

    // std::string file_name = generateTempFileName("map.bin");
    // std::cout << file_name << std::endl;
    // write(file_name, *map);  // we do not need a projector to write to bin
    // // if the map could not be parsed, exceptoins are thrown. Alternatively, you can provide an
    // // error struct. Then lanelet2 will load the map as far as possible and write all the errors
    // // that occured to the error object that you passed:
    // map = load(exampleMapPath, projector, &errors);
    // assert(errors.empty());  // of no errors occurred, the map could be fully parsed.
    // printf("lanelet size[%d], initialized\n", int32_t(map->laneletLayer.size()));
}

std::string HDMap::generateTempFileName(const std::string& name)
{
    char tmpDir[] = "/tmp/lanelet2_mapXXXXXX";
    auto* file = mkdtemp(tmpDir);
    if (file == nullptr)
    {
        throw lanelet::IOError("Failed to open a temporary file for writing");
    }
    std::cout << std::string(file) + '/' + name << std::endl;
    return std::string(file) + '/' + name;
}

}  // namespace cat
