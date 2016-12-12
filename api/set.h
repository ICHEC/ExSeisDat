#include "cfileapi.h"
extern "C"
{
typedef struct ExSeisSetWrapper * ExSeisSet;
extern ExSeisSet makeSet(ExSeisHandle piol, const char * ptrn);
extern void dropSet(ExSeisSet s);
extern void getMinMaxSet(ExSeisSet s, Meta m1, Meta m2, CoordElem * minmax);
//extern void sortSet(ExSeisSet s, bool (* func)(const Param *, const Param *));
extern void defsortSet(ExSeisSet s, SortType type);
extern size_t getInNt(ExSeisSet s);
extern size_t getLNtSet(ExSeisSet s);
extern void outputSet(ExSeisSet s, const char * oname);
extern void textSet(ExSeisSet s, const char * outmsg);
extern void summarySet(ExSeisSet s);
extern void addSet(ExSeisSet s, const char * name);
}
