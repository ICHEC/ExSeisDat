#include "mockparam.hh"

namespace PIOL {

MockParam& mockParam()
{
    static MockParam mockParam;
    return mockParam;
}

MockParamFreeFunctions& mockParamFreeFunctions()
{
    static MockParamFreeFunctions mockParamFreeFunctions;
    return mockParamFreeFunctions;
}

}
