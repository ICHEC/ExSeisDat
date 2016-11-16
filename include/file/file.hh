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

struct Rule;
/*! Derived class for initialising the trace parameter structure
 *  and storing a structure with the necessary rules.
 */
struct Param
{
    std::vector<geom_t> f;    //!< Floating point array.
    std::vector<llint> i;     //!< Integer array.
    std::vector<short> s;     //!< Short array.
    std::vector<size_t> t;    //!< trace number array.
    std::shared_ptr<Rule> r;  //!< The rules which describe the indexing of the arrays.

    /*! Allocate the basic space required to store the arrays and store the rules.
     *  \param[in] r_ The rules which describe the layout of the arrays.
     *  \param[in] sz The number of sets of trace parameters.
     */
    Param(std::shared_ptr<Rule> r_, size_t sz);
    /*! Allocate the basic space required to store the arrays and store the rules. Default rules
     *  \param[in] sz The number of sets of trace parameters.
     */
    Param(size_t sz);

    /*! Return the number of sets of trace parameters.
     *  \return Number of sets
     */
    size_t size(void)
    {
        return t.size();
    }

    /*! Less-than operator. An operator overload required for template subsitution
     *  \param[in] a The Param object to compare with
     *  \return Return true if the index of the current object is less than
     *  the right operand.
     */
    bool operator<(Param & a)
    {
        return f.size() < a.f.size();   //Arbitrary function
    }

    /*! Equality operator
     * \param[in] p Param Structure to compare with.
     * \return Return true if the structures are equivalent.
     */
    bool operator==(Param & p) const;

    /*! Not-Equal operator
     * \param[in] p Param Structure to compare with.
     * \return Return true if the structures are equivalent.
     */
    bool operator!=(Param & p) const
    {
        return !this->operator==(p);
    }
};

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

extern const Param * PARAM_NULL;    //!< The NULL parameter so that the correct internal read pattern is selected
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
    virtual size_t readNt(void) = 0;

    /*! \brief Read the number of increment between trace samples
     *  \return The increment between trace samples
     */
    virtual geom_t readInc(void) const;

    /*! \brief Write the human readable text from the file.
     *  \param[in] text_ The new string containing the text (in ASCII format).
     */
    virtual void writeText(const std::string text_) = 0;

    /*! \brief Write the number of samples per trace
     *  \param[in] ns_ The new number of samples per trace.
     */
    virtual void writeNs(csize_t ns_) = 0;

    /*! \brief Write the number of traces in the file
     *  \param[in] nt_ The new number of traces.
     */
    virtual void writeNt(csize_t nt_) = 0;

    /*! \brief Write the number of increment between trace samples.
     *  \param[in] inc_ The new increment between trace samples.
     */
    virtual void writeInc(const geom_t inc_) = 0;


    /*! \brief Write the trace parameters from offset to offset+sz to the respective
     *  trace headers.
     *  \param[in] offset The starting trace number.
     *  \param[in] sz The number of traces to process.
     *  \param[in] prm An array of the parameter structures (size sizeof(Param)*sz)
     *
     *  \details It is assumed that this operation is not an update. Any previous
     *  contents of the trace header will be overwritten.
     */
    virtual void writeParam(csize_t offset, csize_t sz, const Param * prm = PARAM_NULL) = 0;

    /*! \brief Read the trace parameters from offset to offset+sz of the respective
     *  trace headers.
     *  \param[in] offset The starting trace number.
     *  \param[in] sz The number of traces to process.
     *  \param[in] prm An array of the parameter structures (size sizeof(Param)*sz)
     */
    virtual void readParam(csize_t offset, csize_t sz, Param * prm  = const_cast<Param *>(PARAM_NULL)) const = 0;

    /*! \brief Read the traces from offset to offset+sz
     *  \param[in] offset The starting trace number.
     *  \param[in] sz The number of traces to process.
     *  \param[out] trace The array of traces to fill from the file
     *  \param[out] prm A contiguous array of the parameter structures (size sizeof(Param)*sz)
     */
    virtual void readTrace(csize_t offset, csize_t sz, trace_t * trace, Param * prm = const_cast<Param *>(PARAM_NULL)) const = 0;

    /*! \brief Write the traces from offset to offset+sz
     *  \param[in] offset The starting trace number.
     *  \param[in] sz The number of traces to process.
     *  \param[in] trace The array of traces to write to the file
     *  \param[in] prm A contiguous array of the parameter structures (size sizeof(Param)*sz)
     */
    virtual void writeTrace(csize_t offset, csize_t sz, trace_t * trace, const Param * prm = PARAM_NULL) = 0;

    /*! \brief Read the traces specified by the offsets in the passed offset array.
     *  \param[in] sz The number of traces to process
     *  \param[in] offset An array of trace numbers to read.
     *  \param[out] trace A contiguous array of each trace (size sz*ns*sizeof(trace_t))
     *  \param[out] prm A parameter structure
     *
     *  \details When prm==PARAM_NULL only the trace DF is read.
     */
    virtual void readTrace(csize_t sz, csize_t * offset, trace_t * trace, Param * prm = const_cast<Param *>(PARAM_NULL)) const = 0;

    /*! \brief Write the traces specified by the offsets in the passed offset array.
     *  \param[in] sz The number of traces to process
     *  \param[in] offset An array of trace numbers to write.
     *  \param[in] trace A contiguous array of each trace (size sz*ns*sizeof(trace_t))
     *  \param[in] prm A parameter structure
     *
     *  \details When prm==PARAM_NULL only the trace DF is written.
     *  It is assumed that the parameter writing operation is not an update. Any previous
     *  contents of the trace header will be overwritten.
     */
    virtual void writeTrace(csize_t sz, csize_t * offset, trace_t * trace, const Param * prm = PARAM_NULL) = 0;

    /*! \brief Read the traces specified by the offsets in the passed offset array.
     *  \param[in] sz The number of traces to process
     *  \param[in] offset An array of trace numbers to read.
     *  \param[out] prm A parameter structure
     */
    virtual void readParam(csize_t sz, csize_t * offset, Param * prm) const = 0;

    /*! \brief Write the traces specified by the offsets in the passed offset array.
     *  \param[in] sz The number of traces to process
     *  \param[in] offset An array of trace numbers to write.
     *  \param[in] prm A parameter structure
     *
     *  \details It is assumed that the parameter writing operation is not an update. Any previous
     *  contents of the trace header will be overwritten.
     */
    virtual void writeParam(csize_t sz, csize_t * offset, const Param * prm) = 0;
};
}}
#endif
