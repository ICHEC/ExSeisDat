#ifndef PIOLCSET_INCLUDE_GUARD
#define PIOLCSET_INCLUDE_GUARD
#include "cfileapi.h"
#ifdef __cplusplus
extern "C"
{
#endif
typedef struct ExSeisSetWrapper * ExSeisSet;
extern ExSeisSet initSet(ExSeisHandle piol, const char * ptrn);
extern void freeSet(ExSeisSet s);
extern void getMinMaxSet(ExSeisSet s, Meta m1, Meta m2, CoordElem * minmax);
//extern void sortSet(ExSeisSet s, bool (* func)(const Param *, const Param *));
extern void sortSet(ExSeisSet s, SortType type);
extern size_t getInNt(ExSeisSet s);
extern size_t getLNtSet(ExSeisSet s);
extern void outputSet(ExSeisSet s, const char * oname);
extern void textSet(ExSeisSet s, const char * outmsg);
extern void summarySet(ExSeisSet s);
extern void addSet(ExSeisSet s, const char * name);
#ifdef __cplusplus
}
#endif
#endif
