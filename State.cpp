/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "state.h"
#include <iostream>

namespace ems
{
  namespace Scavenger
  {
      
 State::State(){
      
      };
      State::State(const State &rhs)
    {
      *this = rhs;
    }
      State::~State()
    {
      /* empty */
    }
      State &State::operator=(const State &rhs)
    {
      this->mX = rhs.mX;
      this->mY = rhs.mY;
      this->mZ = rhs.mZ;
      this->mCharge = rhs.mCharge;
      this->mHP = rhs.mHP;
      return *this;
    }
      
    bool State::operator==(const State &rhs) const
    {
      
      return (this->mX == rhs.mX && this->mY == rhs.mY);
    }
      
    bool State::operator<(const State &rhs) const
    {
      if(this->mX < rhs.mX) { return true; }
      if(this->mX > rhs.mX) { return false; }
      if(this->mY < rhs.mY) { return true; }
      return false;
    }
      void State::Display() const
    {
      std::cout << "x = " << this->mX  <<std::endl << "y = " << this->mY <<std::endl << "z = " << mZ << std::endl << "charge =  " << this->mCharge << std::endl << 
               "hit points = " << mHP << std::endl;
    }
      
    bool State::IsEqual(const ai::Search::State * const state_in) const
    {
      const State * const state = dynamic_cast<const State * const>(state_in);
      return (*this) == (*state);
    }
      
    bool State::IsLessThan(const ai::Search::State * const state_in) const
    {
      const State * const state = dynamic_cast<const State * const>(state_in);
      return (*this) < (*state);
    }
    Cell * State::GetCell() const
    {
        return mCell;
    }
    void State::SetCell(Cell &cell)
    {
        mCell = &cell;
    }
    double State::GetX() const
    {
      return this->mX;
    }
    double State::GetY() const
    {
      return this->mY;
    }
    double State::GetCharge() const
    {
      return this->mCharge;
    }

    bool State::SetX(const double &x_in)
    {
      this->mX = x_in;
      return true;
    }
    bool State::SetY(const double &y_in)
    {
      this->mY = y_in;
      return true;
    }
    bool State::SetCharge(const double &c_in)
    {
      this->mCharge = c_in;
      return true;
    }
  }
}