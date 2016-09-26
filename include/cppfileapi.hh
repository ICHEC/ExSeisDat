#include "global.hh"
#include "file/file.hh"
namespace PIOL {
namespace File { class Direct; }
class ExSeis
{
    Piol piol;

    public :
    ExSeis(const Log::Verb maxLevel = Log::Verb::None);

    ExSeis(bool initComm, const Log::Verb maxLevel = Log::Verb::None);

    operator ExSeisPIOL * ()
    {
        return piol.get();
    }

    operator Piol ()
    {
        return piol;
    }

    size_t getRank(void)
    {
        return piol->comm->getRank();
    }

    size_t getNumRank(void)
    {
        return piol->comm->getNumRank();
    }

    void barrier(void) const
    {
        piol->comm->barrier();
    }

    /*! \brief A function to check if an error has occured in the PIOL. If an error has occured the log is printed, the object destructor is called
     *  and the code aborts.
     * \param[in] msg A message to be printed to the log.
     */
    void isErr(const std::string msg = "") const;
    friend class File::Direct;
};

namespace File {
class Direct : public Interface
{
    std::shared_ptr<Interface> file;

    public :
    template <class F, class O, class D>
    Direct(const Piol piol_, const std::string name_,
                            const F & f, const O & o,
                            const D & d, FileMode mode) : Interface(piol_, name_, nullptr)
    {
#warning add Unit test for D::Type
        auto data = std::make_shared<D::Type>(piol, name, d, mode);
        auto obj = std::make_shared<O::Type>(piol, name, o, data, mode);
        file = std::make_shared<F::Type>(piol, name, f, obj, mode);
    }

    Direct(const Piol piol_, const std::string name_, FileMode mode = FileMode::Read);

    ~Direct(void) { }

    Interface * operator->() const
    {
        return file.get();
    }

    const std::string & readText(void) const;
    size_t readNs(void) const;
    size_t readNt(void) const;
    geom_t readInc(void) const;
    void writeText(const std::string text_);
    void writeNs(const size_t ns_);
    void writeNt(const size_t nt_);
    void writeInc(const geom_t inc_);
    void readCoordPoint(const Coord item, csize_t i, csize_t sz, coord_t * buf) const;
    void readGridPoint(const Grid item, csize_t i, csize_t sz, grid_t * buf) const;
    void writeTraceParam(csize_t offset, csize_t sz, const TraceParam * prm);
    void readTraceParam(csize_t offset, csize_t sz, TraceParam * prm) const;
    void readTrace(csize_t offset, csize_t sz, trace_t * trace) const;
    void writeTrace(csize_t offset, csize_t sz, trace_t * trace);
};
}}
