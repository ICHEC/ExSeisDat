////////////////////////////////////////////////////////////////////////////////
/// @file
/// @details Primary C++ API header
////////////////////////////////////////////////////////////////////////////////
#ifndef EXSEISDAT_PIOL_HH
#define EXSEISDAT_PIOL_HH

/// @namespace PIOL
/// @brief The PIOL namespace is the base namespace for the ExSeisPIOL library.

#include "ExSeisDat/PIOL/CommunicatorInterface.hh"
#include "ExSeisDat/PIOL/CommunicatorMPI.hh"
#include "ExSeisDat/PIOL/DataInterface.hh"
#include "ExSeisDat/PIOL/DataMPIIO.hh"
#include "ExSeisDat/PIOL/ExSeis.hh"
#include "ExSeisDat/PIOL/ExSeisPIOL.hh"
#include "ExSeisDat/PIOL/Logger.hh"
#include "ExSeisDat/PIOL/Meta.h"
#include "ExSeisDat/PIOL/Model3dInterface.hh"
#include "ExSeisDat/PIOL/ObjectInterface.hh"
#include "ExSeisDat/PIOL/ObjectSEGY.hh"
#include "ExSeisDat/PIOL/Param.h"
#include "ExSeisDat/PIOL/ReadDirect.hh"
#include "ExSeisDat/PIOL/ReadInterface.hh"
#include "ExSeisDat/PIOL/ReadModel.hh"
#include "ExSeisDat/PIOL/ReadSEGY.hh"
#include "ExSeisDat/PIOL/ReadSEGYModel.hh"
#include "ExSeisDat/PIOL/Rule.hh"
#include "ExSeisDat/PIOL/RuleEntry.hh"
#include "ExSeisDat/PIOL/SEGYRuleEntry.hh"
#include "ExSeisDat/PIOL/SortType.h"
#include "ExSeisDat/PIOL/TaperType.h"
#include "ExSeisDat/PIOL/Tr.h"
#include "ExSeisDat/PIOL/Verbosity.h"
#include "ExSeisDat/PIOL/WriteDirect.hh"
#include "ExSeisDat/PIOL/WriteInterface.hh"
#include "ExSeisDat/PIOL/WriteSEGY.hh"
#include "ExSeisDat/PIOL/character_encoding.hh"
#include "ExSeisDat/PIOL/constants.hh"
#include "ExSeisDat/PIOL/decompose.h"
#include "ExSeisDat/PIOL/makeFile.hh"
#include "ExSeisDat/PIOL/number_encoding.hh"
#include "ExSeisDat/PIOL/operations/gather.hh"
#include "ExSeisDat/PIOL/operations/minmax.h"
#include "ExSeisDat/PIOL/operations/sort.hh"
#include "ExSeisDat/PIOL/operations/taper.hh"
#include "ExSeisDat/PIOL/operations/temporalfilter.hh"
#include "ExSeisDat/PIOL/param_utils.hh"
#include "ExSeisDat/PIOL/segy_utils.hh"
#include "ExSeisDat/utils/Distributed_vector.hh"
#include "ExSeisDat/utils/gain_control/AGC.h"
#include "ExSeisDat/utils/mpi/MPI_error_to_string.hh"
#include "ExSeisDat/utils/typedefs.h"

#endif  // EXSEISDAT_PIOL_HH
