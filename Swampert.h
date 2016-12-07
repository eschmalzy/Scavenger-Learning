#ifndef _SWAMPERT_H_
#define _SWAMPERT_H_
#include <ai_scavenger.h>
#include <ai_search.h>
#include <map>
#include "cell.h"
#include  "model.h"
#include "problem.h"

namespace ems
{
  namespace Scavenger
  {
    class Swampert : public ai::Agent::AgentProgram
    {
    public:
        Swampert();
      Swampert(ai::Agent::Options *opts);
      ~Swampert();
      virtual ai::Agent::Action * Program(const ai::Agent::Percept * percept);
      std::pair<double, double> Coordinates(double x, double y);
      bool LookNorth(const ai::Agent::Percept * percept);
      bool LookSouth(const ai::Agent::Percept * percept);
      bool LookEast(const ai::Agent::Percept * percept);
      bool LookWest(const ai::Agent::Percept * percept);
      void VisitCell(Cell * c, const ai::Agent::Percept *percept);
      Cell * getCell(std::pair<double, double> pair);
      void parsePercepts(const ai::Agent::Percept * percept, bool recharge);
      double getCharge(){return mCharge_num;};
      double getHitPoints(){return mHp_num;};
      double getX(){return mX;};
      double getY(){return mY;};
      double getZ(){return mZ;};
      double getNX(){return mNX;};
      double getNY(){return mNY;};
      std::map <std::pair<double, double>, Cell *> getMCells(){return mCells;};
      void setCharge(double c){mCharge_num = c;};
      void setHP(double hp){mHp_num = hp;};
      void setX(double x){mX = x;};
      void setY(double y){mX = y;};
      void setZ(double z){mX = z;};
      void setNX(double nx){mX = nx;};
      void setNY(double ny){mX = ny;};
      std::string getNorthInterface(){return mNorthInterface;};
      std::string getSouthInterface(){return mSouthInterface;};
      std::string getEastInterface(){return mEastInterface;};
      std::string getWestInterface(){return mWestInterface;};
      void setNorthInterface(std::string n){mNorthInterface = n;};
      void setSouthInterface(std::string s){mSouthInterface = s;};
      void setEastInterface(std::string e){mEastInterface = e;};
      void setWestInterface(std::string w){mWestInterface = w;};
      Model * getModel(){return mModel;};
      void setModel(Model * m){mModel = m;};
      void setMCells(std::map <std::pair<double, double>, Cell *> cells){mCells = cells;};
    protected:
          Model * mModel;
          std::map <std::pair<double, double>, Cell *> mCells;
        double mCharge_num = 0, mHp_num = 0,mGoalx = 0, mGoaly = 0, mGoalz = 0, mX = 0, mY = 0, mZ = 0, mNX = 0, mNY = 0;   
        int mBase_num = 0,mCell_num = 0;
        std::pair<double,double> mCoordinates;
        std::pair<double, double> mNew_coordinates;
        std::string mNorthInterface;std::string mSouthInterface;std::string mEastInterface;std::string mWestInterface;
        State * mState;

    private:
    };
    class Action : public ai::Search::Action
    {
    public:
      enum
        {
          /* Movement actions */
          GO_NORTH,
          GO_SOUTH,
          GO_EAST,
          GO_WEST,
          /* Look at the horizon in one direction */
          LOOK,

          /* others */
          RECHARGE,
          PICKUP,  /* Attempt to put an object in the bin */
          DROP,    /* Attempt to remove an object from the bin */
          DEPOSIT, /* Attempt to put an object from the bin to the base */
          EXAMINE, /* Examine the details of object */
          QUIT,    /* Stop playing */
        };
        
      Action();
      Action(const Action &rhs);
      virtual ~Action();
      Action &operator=(const Action &rhs);
      bool operator==(const Action &rhs) const;
      virtual void Display() const;
      bool SetType(int type_in);
      int GetType() const;
        
    protected:
      int type; /* A_* */

    private:
    };
  }
}

#endif /* _Swampert_H_ */