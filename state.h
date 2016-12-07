
#ifndef _STATE_H
#define _STATE_H

#include <ai_search.h>
#include "cell.h"

namespace ems
{
  namespace Scavenger
  {
      
        class State : public ai::Search::State
              {
              public:
                State();
              State(const State &rhs);
              virtual ~State();
              State &operator=(const State &rhs);
              bool operator==(const State &rhs) const;
              bool operator<(const State &rhs) const;
              virtual void Display() const;

              virtual bool IsEqual(const ai::Search::State * const state_in) const;

              virtual bool IsLessThan(const ai::Search::State * const state_in) const;

              double GetX() const;
              double GetY() const;
              double GetZ() const {return mZ;};
              double GetHP() const {return mHP;};
              double GetCharge() const;
              double GetCellNum() const {return mCellNum;};
              void SetCell(Cell &cell);
              Cell * GetCell() const;
              bool SetX(const double &x_in);
              bool SetY(const double &y_in);
              bool SetZ(const double &z_in) {mZ = z_in; return true;};
              bool SetCellNum(const int &cell_in) {mCellNum = cell_in; return true;};
              bool SetHP(const double &hp_in) {mHP = hp_in; return true;};
              bool SetCharge(const double &c_in);
              Cell * mCell;

            protected:
                double mX;
                double mY, mZ;
                int mCellNum;
                double mCharge, mHP;
              /* Choose data values appropriate for your problem.
               */

            private:
        };
  }
}
#endif /* STATE_H */

