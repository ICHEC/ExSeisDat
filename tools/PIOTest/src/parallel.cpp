/* Cathal O Broin - cathal.obroin4 at mail.dcu.ie - 2015-2016
   This work is not developed in affiliation with any organisation.
  
   This file is part of AILM.

   AILM is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   AILM is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with AILM.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <algorithm>
#include "parallel.hh"
namespace parallel
{
std::pair<size_t, size_t> distrib(size_t Rank, size_t NProcs, size_t WorkUnits)
{
    size_t DivR = WorkUnits%NProcs;
    size_t DivNum = WorkUnits/NProcs;
    size_t DivStart = DivNum * Rank + std::min(size_t(Rank), DivR);
    DivNum = std::min(WorkUnits - DivStart, DivNum + (Rank < DivR));

    std::pair<size_t, size_t> DivLab(DivStart, DivStart + DivNum);
    return DivLab;
}
}

