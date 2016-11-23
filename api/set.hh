/*******************************************************************************************//*!
 *   \file
 *   \author Cathal O Broin - cathal@ichec.ie - first commit
 *   \copyright TBD. Do not distribute
 *   \date November 2016
 *   \brief The Set layer interface
*//*******************************************************************************************/
#ifndef APIPIOLSET_INCLUDE_GUARD
#define APIPIOLSET_INCLUDE_GUARD
class Set : public InternalSet
{

    public :
    void sort(SortType type);
};

#endif
