/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date July 2016
 *   \brief The File layer interface
 *   \details The File layer interface is a base class which specific File implementations
 *   work off
*//*******************************************************************************************/
#ifndef PIOLFILE_INCLUDE_GUARD
#define PIOLFILE_INCLUDE_GUARD
#include "global.hh"

namespace PIOL { namespace File {

/*! A structure composed of two coordinates to form a single coordinate point
 */
struct coord_t
{
    geom_t x;   //!< The x coordinate
    geom_t y;   //!< The y coordinate

    /*! Constructor for initialising coordinates
     *  \param[in] x_ intialisation value for x
     *  \param[in] y_ intialisation value for x
     */
    coord_t(const geom_t x_, const geom_t y_) : x(x_), y(y_) { }
    /*! Default constructor (set both coordinates to 0)
     */
    coord_t() : x(geom_t(0)), y(geom_t(0)) { }
};

/*! A structure composed of two grid values to form a single grid point
 */
struct grid_t
{
    llint il;   //!< The inline value
    llint xl;   //!< The crossline value

    /*! Constructor for initialising a grid point
     *  \param[in] i_ intialisation value for il
     *  \param[in] x_ intialisation value for xl
     */
    grid_t(const geom_t i_, const geom_t x_) : il(i_), xl(x_) { }

    /*! Default constructor (set both grid values to 0)
     */
    grid_t() : il(llint(0)), xl(llint(0)) { }
};

/*! \brief Possible coordinate sets
 */
enum class Coord : size_t
{
    Src,    //!< Source Coordinates
    Rcv,    //!< Receiver Coordinates
    CMP     //!< Common Midpoint Coordinates
};

/*! \brief Possible Grids
 */
enum class Grid : size_t
{
    Line    //!< Inline/Crossline grid points
};

/*! \brief This structure holds all relevant trace parameters for describing a single trace
 *  excluding what is contained in the header.
 */
struct TraceParam
{
    coord_t src;    //!< The Source coordinate
    coord_t rcv;    //!< The Receiver coordinate
    coord_t cmp;    //!< The common midpoint
    grid_t line;    //!< The line coordinates (il, xl)
    size_t tn;      //!< TODO: Add unit test
};

/*! \brief The File layer interface. Specific File implementations
 *  work off this base class.
 */
class Interface
{
    protected :
    Piol piol;                            //!< The PIOL object.
    std::string name;                     //!< Store the file name for debugging purposes.
    std::shared_ptr<Obj::Interface> obj;  //!< Pointer to the Object-layer object (polymorphic).
    size_t ns;                            //!< The number of samples per trace.
    size_t nt;                            //!< The number of traces.
    std::string text;                     //!< Human readable text extracted from the file
    geom_t inc;                           //!< The increment between samples in a trace

    public :
    /*! \brief The constructor.
     *  \param[in] piol_ This PIOL ptr is not modified but is used to instantiate another shared_ptr.
     *  \param[in] name_ The name of the file associated with the instantiation.
     *  \param[in] obj_  Pointer to the Object-layer object (polymorphic).
     */
    Interface(const Piol piol_, const std::string name_, std::shared_ptr<Obj::Interface> obj_) : piol(piol_), name(name_), obj(obj_)
    {
    }
    Interface(void) { }
    /*! \brief A virtual destructor to allow deletion, unique_pr polymorphism.
     */
    virtual ~Interface(void) { }

    /*! \brief Read the human readable text from the file
     *  \return A string containing the text (in ASCII format)
     */
    virtual const std::string & readText(void) const;

    /*! \brief Read the number of samples per trace
     *  \return The number of samples per trace
     */
    virtual size_t readNs(void) const;

    /*! \brief Read the number of traces in the file
     *  \return The number of traces
     */
    virtual size_t readNt(void) const;

    /*! \brief Read the number of increment between trace samples
     *  \return The increment between trace samples
     */
    virtual geom_t readInc(void) const;

    /*! \brief Pure virtual function to read coordinate pairs from the ith-trace to i+sz.
     *  \param[in] item The coordinate pair of interest
     *  \param[in] i The starting trace number
     *  \param[in] sz The number of traces to process
     *  \param[out] buf The buffer
     */
    virtual void readCoordPoint(const Coord item, csize_t i, csize_t sz, coord_t * buf) const = 0;

    /*! \brief Pure virtual function to read grid pairs from the ith-trace to i+sz.
     *  \param[in] item The grid pair of interest
     *  \param[in] i The starting trace number
     *  \param[in] sz The number of traces to process
     *  \param[out] buf The buffer
     */
    virtual void readGridPoint(const Grid item, csize_t i, csize_t sz, grid_t * buf) const = 0;

    /*! \brief Pure virtual function to write the trace parameters from offset to offset+sz to the respective
     *  trace headers.
     *  \param[in] offset The starting trace number.
     *  \param[in] sz The number of traces to process.
     *  \param[in] prm An array of the parameter structures (size sizeof(TraceParam)*sz)
     *
     *  \details It is assumed that this operation is not an update. Any previous
     *  contents of the trace header will be overwritten.
     */
    virtual void writeTraceParam(csize_t offset, csize_t sz, const TraceParam * prm) = 0;

    /*! \brief Pure virtual function to read the trace parameters from offset to offset+sz of the respective
     *  trace headers.
     *  \param[in] offset The starting trace number.
     *  \param[in] sz The number of traces to process.
     *  \param[in] prm An array of the parameter structures (size sizeof(TraceParam)*sz)
     */
    virtual void readTraceParam(csize_t offset, csize_t sz, TraceParam * prm) const = 0;

    /*! \brief Pure virtual function to read the traces from offset to offset+sz
     *  \param[in] offset The starting trace number.
     *  \param[in] sz The number of traces to process.
     *  \param[out] trace The array of traces to fill from the file
     */
    virtual void readTrace(csize_t offset, csize_t sz, trace_t * trace) const = 0;

    /*! \brief Pure virtual function to write the traces from offset to offset+sz
     *  \param[in] offset The starting trace number.
     *  \param[in] sz The number of traces to process.
     *  \param[in] trace The array of traces to write to the file
     */
    virtual void writeTrace(csize_t offset, csize_t sz, trace_t * trace) = 0;

    /*! \brief Pure virtual function to write the human readable text from the file.
     *  \param[in] text_ The new string containing the text (in ASCII format).
     */
    virtual void writeText(const std::string text_) = 0;

    /*! \brief Pure virtual function to write the number of samples per trace
     *  \param[in] ns_ The new number of samples per trace.
     */
    virtual void writeNs(const size_t ns_) = 0;

    /*! \brief Pure virtual function to write the number of traces in the file
     *  \param[in] nt_ The new number of traces.
     */
    virtual void writeNt(const size_t nt_) = 0;

    /*! \brief Pure virtual function to write the number of increment between trace samples.
     *  \param[in] inc_ The new increment between trace samples.
     */
    virtual void writeInc(const geom_t inc_) = 0;
};
}}
#endif
