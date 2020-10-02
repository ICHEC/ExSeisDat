#include "sglobal.hh"

#include "exseisdat/piol/configuration/ExSeis.hh"
#include "exseisdat/piol/file/Input_file_segy.hh"
#include "exseisdat/piol/operations/minmax.hh"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>

using namespace exseis::utils;
using namespace exseis::piol;

/*! Create the initial KML file settings to order to describe the output in
 *  general
 *  @param[in,out] file file handle opened file is assigned to
 *  @param[in] oname name of the file to open
 *  @param[in] folder KML folder name containing all trace lines
 */
void init_kml(std::ofstream& file, std::string oname, std::string folder)
{
    file.open(oname);
    file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    file << "<kml xmlns=\"http://www.opengis.net/kml/2.2\"\n";
    file << " xmlns:gx=\"http://www.google.com/kml/ext/2.2\"\n";
    file << " xmlns:kml=\"http://www.opengis.net/kml/2.2\"\n";
    file << " xmlns:atom=\"http://www.w3.org/2005/Atom\">\n\n";
    file << "<Document>\n<Folder>\n<name>" + folder
                + "</name>\n<Style id=\"pathstyle\">\n\t<LineStyle>\n\t\t";
    file
        << "<color>ff190cff</color>\n\t\t<width>2</width>\n\t</LineStyle>\n</Style>\n";
    file
        << "<Schema name=\"Intrepid\" id=\"Intrepid\">\n\t<SimpleField name=\"Name\" type=\"string\"></SimpleField>";
    file
        << "\n\t<SimpleField name=\"Description\" type=\"string\"></SimpleField>\n</Schema>\n\n";
}

/*! Creates a high precision 10 decimals() string from a single coordinate
 *  (i.e. not a pair)
 *  @param[in] coord lat/long coords to change to string
 *  @return the coordinates as a string
 */
std::string high_prec_str(exseis::utils::Floating_point coord)
{
    return std::to_string(static_cast<long long>(std::floor(coord))) + "."
           + std::to_string(static_cast<long long>(
               std::floor((coord - std::floor(coord)) * 10000000000)));
}

/*! Add a line containing coordinates data to the KML file.
 *  @param[in,out] file file handle to write to
 *  @param[in] name name to be given to this set of coordinates
 *  @param[in] coords the lat/long coords to print to the file
 */
void add_line(
    std::ofstream& file, std::string name, std::vector<CoordElem> coords)
{
    file
        << "<Placemark>\n\t<name>" + name
               + "</name>\n\t<styleUrl>#pathstyle</styleUrl>\n\t<LineString>\n\t\t";
    file
        << "<ExtendedData>\n\t\t\t<SchemaData schemaUrl=\"#Intrepid\">\n\t\t\t\t";
    file
        << "<SimpleData name=\"Name\">" + name
               + "</SimpleData>\n\t\t\t</SchemaData>\n\t\t</ExtendedData>\n\t\t\t";
    file << "<tessellate>1</tessellate>\n\t\t<coordinates>\n\t\t\t"
                + high_prec_str(coords[2].val) + ","
                + high_prec_str(coords[0].val);
    file << +",0\n\t\t\t" + high_prec_str(coords[3].val) + ","
                + high_prec_str(coords[1].val) + ",0\n\t\t </coordinates>\n\t";
    file << "</LineString>\n</Placemark>\n";
}

/*! close the kml outpuut file
 *  @param[in,out] file file handle to close
 */
void close_kml(std::ofstream& file)
{
    file << "</Folder>\n</Document>\n</kml>";
    file.close();
}

/*! Compute a lat long value from a UTM input
 *  @param[in] easting UTM easting coordinate
 *  @param[in] northing UTM northing coordinate
 *  @param[in] utm_zone the UTM zone the coordinates lie in
 *  @param[out] lat latitude coordinate
 *  @param[out] lng longitude coordinate
 *
 *  @details Formula is from
 *          https://www.uwgb.edu/dutchs/UsefulData/UTMFormulas.HTM
 *          (Excel Spreadsheet is clearer than formula)
 */
void utm_to_lat_long(
    exseis::utils::Floating_point easting,
    exseis::utils::Floating_point northing,
    std::string utm_zone,
    exseis::utils::Floating_point& lat,
    exseis::utils::Floating_point& lng)
{
    exseis::utils::Floating_point hemi =
        (std::toupper(utm_zone.back()) == 'N' ? 1 : -1);
    exseis::utils::Floating_point num_zone = std::stof(utm_zone);

    exseis::utils::Floating_point const eq_rad  = 6378137;
    exseis::utils::Floating_point const pol_rad = 6356752;
    exseis::utils::Floating_point const k0      = 0.9996;
    exseis::utils::Floating_point e =
        std::sqrt(1 - (pol_rad * pol_rad) / (eq_rad * eq_rad));
    exseis::utils::Floating_point ei =
        (1 - std::sqrt(1 - e * e)) / (1 + std::sqrt(1 - e * e));
    exseis::utils::Floating_point x = 500000 - easting;
    exseis::utils::Floating_point mu =
        (northing / k0)
        / (eq_rad
           * (1 - std::pow(e, 2U) / 4 - 3 * std::pow(e, 4U) / 64
              - 5 * std::pow(e, 6U) / 256));
    exseis::utils::Floating_point phi =
        mu + (3 * ei / 2 - 27 * std::pow(ei, 3) / 32) * std::sin(2 * mu)
        + (21 / 16 * std::pow(ei, 2U) - 55 / 32 * std::pow(ei, 4U))
              * std::sin(4 * mu)
        + (151 / 96 * std::pow(ei, 3U) - 417 / 128 * std::pow(ei, 5U))
              * std::sin(6 * mu)
        + (1097 / 512 * std::pow(ei, 4U)) * std::sin(8 * mu);
    exseis::utils::Floating_point c =
        (e * e / (1 - e * e)) * std::pow(std::cos(phi), 2);
    exseis::utils::Floating_point d =
        x / (k0 * eq_rad / std::sqrt(1 - std::pow(e * std::sin(phi), 2)));
    lat = (180 * hemi / std::acos(-1))
          * (phi
             - ((1 - std::pow(e * std::sin(phi), 2)) * std::tan(phi)
                / (1 - e * e))
                   * ((d * d / 2)
                      + ((5 + 3 * std::pow(std::tan(phi), 2) + 10 * c
                          - 4 * c * c - 9 * (e * e / (1 - e * e)))
                         * std::pow(d, 4) / 24)
                      + ((61 + 90 * std::pow(std::tan(phi), 2)
                          + 298 * std::pow(std::tan(phi), 4)
                          - 252 * (e * e / (1 - e * e)) - 3 * c * c)
                         * std::pow(d, 6) / 720)));
    lng =
        (6 * num_zone - 183)
        - (180
           * (d
              + ((1 + 2 * std::pow(std::tan(phi), 2) + c) * std::pow(d, 3) / 6)
              + ((5 - 2 * c + 28 * std::pow(std::tan(phi), 2) - 3 * c * c
                  + 24 * std::pow(std::tan(phi), 4) + 8 * (e * e / (1 - e * e)))
                 * std::pow(d, 5) / 120))
           / (std::cos(phi) * std::acos(-1)));
}

/*! Read from the input file. Find the min/max  x_src, y_src, x_rcv, y_rcv, xCmp
 *  and yCMP. Write the matching traces to the output file in that order.
 *  @param[in] piol Piol constructor
 *  @param[in] iname Input file
 *  @param[in] minmax Minimum and maximum coordinates
 */
void calc_min(
    std::shared_ptr<ExSeis> piol,
    std::string iname,
    std::vector<CoordElem>& minmax)
{
    Input_file_segy in(piol, iname);

    auto dec = block_decomposition(
        in.read_nt(), piol->comm->get_num_rank(), piol->comm->get_rank());

    size_t offset = dec.global_offset;
    size_t lnt    = dec.local_size;

    Trace_metadata prm(lnt);
    in.read_param(offset, lnt, &prm);

    get_min_max(
        piol.get(), offset, lnt, Trace_metadata_key::x_src,
        Trace_metadata_key::y_src, prm, minmax.data());
    get_min_max(
        piol.get(), offset, lnt, Trace_metadata_key::x_rcv,
        Trace_metadata_key::y_rcv, prm, minmax.data() + 4U);
}

/* Main function for segy to kml
 *  @param[in] argc The number of input strings.
 *  @param[in] argv The array of input strings.
 *  @return zero on success, non-zero on failure
 *  @details Options:
 *           -i \<file\> : input file name
 *           -o \<file\> : output file name
 *           -f \<folder\> : name of folder in KML file that contains the
 *                           trace lines
 *           -z \<UTMZone\> : UTM Zone if coordinates in UTM
 *           -h \<help\> : prints available command line options
 */
int main(int argc, char** argv)
{
    std::vector<std::string> iname;
    std::string oname;
    std::string utm_zone;
    std::string folder = "SEG-Y";
    bool help          = false;

    std::string opt = "i:o:f:z:h";  // TODO: uses a GNU extension
    for (int c = getopt(argc, argv, opt.c_str()); c != -1;
         c     = getopt(argc, argv, opt.c_str())) {
        switch (c) {
            case 'i':
                iname.push_back(optarg);
                break;

            case 'o':
                oname = optarg;
                break;

            case 'f':
                folder = optarg;
                break;

            case 'z':
                utm_zone = optarg;
                break;

            case 'h':
                help = true;
                break;

                return -1;
        }
    }

    if (help) {
        std::cerr << "Arguments: -i for input file\n";
        std::cerr << "           -o for KML output file\n";
        std::cerr
            << "           -f for name of folder containing trace lines in KML file\n";
        std::cerr << "           -z UTM zone\n";
        std::cerr << "           -help print help text\n";
        return 0;
    }

    if (iname.empty() || oname.empty()) {
        std::cerr << "Invalid arguments given.\n";
        std::cerr << "Arguments: -i for input file, -o for KML output file\n";
        return -2;
    }

    // Expect UTM zone to have the form xS or xN where x is an integer between 1
    // and 60
    if (!utm_zone.empty()
        && ((std::toupper(utm_zone.back()) != 'N'
             && std::toupper(utm_zone.back()) != 'S')
            || (std::stof(utm_zone) < 1 || std::stof(utm_zone) > 60)
            || std::stof(utm_zone) != std::floor(std::stof(utm_zone)))) {
        std::cerr << "Invalid UTM Zone (WGS-84). \n";
        std::cerr
            << "Zones must an integer between 1 and 60 in hemisphere N or S\n";
        return -1;
    }
    else if (utm_zone.empty()) {
        std::cout
            << "No UTM zone specified. Assuming Lat/Long coordinates in input files.\n";
    }

    if (std::toupper(iname[0].back()) != 'Y') {
        std::ifstream file_lst(iname[0]);
        iname.clear();
        std::string sgynm;
        while (std::getline(file_lst, sgynm)) {
            iname.push_back(sgynm);
        }
    }
    std::vector<CoordElem> minmax(8U);

    std::ofstream ofile;
    init_kml(ofile, oname, folder);
    auto piol = ExSeis::make();
    for (size_t i = 0; i < iname.size(); i++) {
        calc_min(piol, iname[i], minmax);
        // If Longitude/Easting is greater than 180, coordinate is in UTM format
        // and must be converted to latitude/longitude (mimimum UTM Easting is
        // 100,000)
        if (minmax[0].val > 180 && !utm_zone.empty()) {
            utm_to_lat_long(
                minmax[0].val, minmax[2].val, utm_zone, minmax[0].val,
                minmax[2].val);
            utm_to_lat_long(
                minmax[1].val, minmax[3].val, utm_zone, minmax[1].val,
                minmax[3].val);
        }
        else if (utm_zone.empty()) {
            std::cerr << "\nError: Invalid coordinates found in file.\n";
            std::cerr << "       Expected lat/long but values must be UTM.\n";
            std::cerr << "       Use the -z option to specify the UTM zone.\n";
        }
        add_line(ofile, iname[i], minmax);
    }
    close_kml(ofile);
    return 0;
}
