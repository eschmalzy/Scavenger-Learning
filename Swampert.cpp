#include <cstdlib>
#include <vector>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <math.h>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>
#include <queue>
#include <time.h>
#include "state.h"
#include "Swampert.h"
//
ems::Scavenger::Problem * mProblem;
ems::Scavenger::Problem * mProblemRecharge;
std::queue<std::string> mPath;
double lastBacktoBaseCost = 0;
std::string gSEEN = "none";
int gObjectsIndex = 0;
int first100randomindex = -1;
int dropoffIndex = 0;
int nomatch = 0;
int first100index = 0;
int valuableDropoffIndex = 0;
double UCSpathCost = 0.0;
int lastsize = 0;
double deduct = 0;
double MARGIN = 1.5 + 2.0 + 1.5;
bool pickupcheck = false;
bool skip = false;
bool first100 = false;
bool semi_known_obj = false;
bool newObject = false;
bool needDropOff = false;
bool pickupLastTime = false;
bool examine = false;
bool need_charge = false;
bool checkDepositValue = false;
bool end = false;
bool exploreMode = true;
int gCount = 0;
int gIndex = 0;
bool gdir = false;
bool gSetStartGoal = true;
bool gohome = false;
bool start = true;
ems::Scavenger::State * startstate = new ems::Scavenger::State();
double gcharge = 0, ghp = 0;
int basex=0,basey=0;
double basez=0;
int gtotaleverything = 0;
int gTotalCount = 0;
int gTotalDeposits = 0;
std::vector<std::string> mObjectIds = {};
std::vector<std::string> mDropoffObjectIds = {};
std::vector<int> mValuableDropoffIds = {};
std::vector<int> randomnums = {};
std::map<std::string, std::string> mObjects = {};
std::vector<double> mOriginTable = {0,0,0};
std::map<std::string, std::vector<std::pair<std::string, int>>> mColors = {};
std::map<std::string, std::vector<std::pair<std::string, int>>> mShapes = {};
std::map<std::string, std::vector<std::pair<std::string, int>>> mSizes = {};
std::map<std::string, std::vector<std::pair<std::string, int>>> mSizes2 = {};
std::map<std::string, std::vector<std::pair<std::string, int>>> mLearnedObjects = {};
std::map<std::string, std::map<std::string, std::string>> mParsedObjects = {}; // obj id is key and value is map of attribute keys to sttribute values

namespace ems
{
  namespace Scavenger
  {
    Swampert::Swampert(){}
    Swampert::Swampert(ai::Agent::Options *opts)
    {
      SetName("Swampert");
      std::cout << "The value of the -U option is: " << opts->GetArgInt("user1") << std::endl;
      srand (time(NULL));
    }

    Swampert::~Swampert()
    {
        delete mModel;
    }
    double Swampert::GetTotals(std::string attr, int origin){
        double total = 0;
        if(attr == "color"){
            std::map<std::string, std::vector<std::pair<std::string, int>>>::const_iterator it;
            for (it = mColors.begin(); it != mColors.end(); it++){
                total += it->second[origin].second;
            }
//            std::cout << "GETTING COUNTS FOR TABLES: COLORS : " << total << std::endl;
        }
        if(attr == "shape"){
            std::map<std::string, std::vector<std::pair<std::string, int>>>::const_iterator it;
            for (it = mShapes.begin(); it != mShapes.end(); it++){
                total += it->second[origin].second;
            }
//            std::cout << "GETTING COUNTS FOR TABLES: SHAPES : " << total << std::endl;
        }
        if(attr == "size"){
            std::map<std::string, std::vector<std::pair<std::string, int>>>::const_iterator it;
            for (it = mSizes.begin(); it != mSizes.end(); it++){
                total += it->second[origin].second;
            }
//            std::cout << "GETTING COUNTS FOR TABLES: SIZES : " << total << std::endl;
        }
        if(attr == "size2"){
            std::map<std::string, std::vector<std::pair<std::string, int>>>::const_iterator it;
            for (it = mSizes2.begin(); it != mSizes2.end(); it++){
                total += it->second[origin].second;
            }
//            std::cout << "GETTING COUNTS FOR TABLES: SIZE2 : " << total << std::endl;
        }
        return total;
    }
    std::vector<double> Swampert::ProbOriginGivenColorShapeSize(std::string size, std::string color, std::string shape){
        double color_G_native = mColors[color][0].second;
        double color_G_mimic = mColors[color][1].second;
        double color_G_earth = mColors[color][2].second;
        double shape_G_native = mShapes[shape][0].second;
        double shape_G_mimic = mShapes[shape][1].second;
        double shape_G_earth = mShapes[shape][2].second;
        double size_G_native = mSizes[size][0].second;
        double size_G_mimic = mSizes[size][1].second;
        double size_G_earth = mSizes[size][2].second;
//        std::cout << color_G_native << " " << color_G_mimic << " " << color_G_earth << " " << shape_G_native << " " << shape_G_mimic << " " << shape_G_earth << " " << size_G_native << " " << size_G_mimic << " " << size_G_earth << std::endl;
//        double size2_G_origin = mSizes2[size2][origin].second;
        double colortotalnative = GetTotals("color", 0);
        double shapetotalnative = GetTotals("shape", 0);
        double sizetotalnative = GetTotals("size", 0);
        double colortotalmimic = GetTotals("color", 1);
        double shapetotalmimic = GetTotals("shape", 1);
        double sizetotalmimic = GetTotals("size", 1);
        double colortotalearth = GetTotals("color", 2);
        double shapetotalearth = GetTotals("shape", 2);
        double sizetotalearth = GetTotals("size", 2);
        double prob = 0;
        double origintotal = mOriginTable[0] + mOriginTable[1] + mOriginTable[2];
        std::vector<double> probs;
        for(double i = 0; i < 3; i++){
            double color_G_origin = mColors[color][i].second;
            double shape_G_origin = mShapes[shape][i].second;
            double size_G_origin = mSizes[size][i].second;
            double colortotal1 = GetTotals("color", i);
            double shapetotal1 = GetTotals("shape", i);
            double sizetotal1 = GetTotals("size", i);
//            std::cout << "WHERE ARE WE NOW? " << mOriginTable[0] << " " << mOriginTable[1] << " " << mOriginTable[2] << std::endl;
            double numerator = ((color_G_origin / colortotal1) * (shape_G_origin / shapetotal1) * (size_G_origin / sizetotal1) * (mOriginTable[i]/origintotal));
            double denom = ((color_G_native / colortotalnative) * (shape_G_native / shapetotalnative) * (size_G_native / sizetotalnative) * (mOriginTable[0]/origintotal));
            double denom2 = ((color_G_mimic / colortotalmimic) * (shape_G_mimic / shapetotalmimic) * (size_G_mimic / sizetotalmimic) * (mOriginTable[1]/origintotal));
            double denom3 = ((color_G_earth / colortotalearth) * (shape_G_earth / shapetotalearth) * (size_G_earth / sizetotalearth) * (mOriginTable[2]/origintotal));
            prob = numerator / (denom + denom2 + denom3);
            probs.push_back(prob);
        }
        return probs;
    }
    //Check the North Interface every time the agent enters the cell
     bool Swampert::LookNorth(const ai::Agent::Percept * percept) {
        //Flag if the previous action was a directinal look, gDir.  gIndex enforces which direction is was.
        if (gdir){
            gdir = false; // reset gDir to false since no action will be taken and I want to advance to next direction
            gIndex = 1; //advance to next Interface
            ai::Agent::PerceptAtom a = percept->GetAtom("LOOK"); // get the percept back from the LOOK action
            mNorthInterface = a.GetValue().c_str();  // set the agents CURRENT interface to this value.
            mNX = mX + 0; // simulate the agent peekiing into the next cell
            mNY = mY + 1000; // simulate the agent peekiing into the next cell
            mNew_coordinates = Coordinates(mNX, mNY);
            std::map<std::pair<int, int>, Cell*>::const_iterator it;
            it = this->mCells.find(mNew_coordinates);// see if this cell was previously seen
            if(it == mCells.end())
            {
                if(mNorthInterface == "cliff"){
                  Cell * c =  new Cell(mCell_num, mNX, mNY, mZ, "", "wall", "", "", 0,1,0,0,0);
                  mCells[mNew_coordinates] = c;
                }
                else if(mNorthInterface == "wall"){
                  Cell * c =  new Cell(mCell_num, mNX, mNY, mZ, "", "cliff", "", "", 0,1,0,0,0);
                  mCells[mNew_coordinates] = c;
                }
                else { Cell *c =  new Cell(mCell_num, mNX, mNY, mZ, "", mNorthInterface, "", "", 0,1,0,0,0);
                    mCells[mNew_coordinates] = c;
                }
                //make a brand new cell.  the only thing I know so far is that the agent's current percept is this next cell's opposite interface
//                                  std::cout << "peek north interfaces unexplored cell created south interface set: " << mNorthInterface << " " << " " << " " << " " << " " << " " << std::endl;
            }
            else
            {
                //if the cell was already in agent memory, update the cell's interface and interface seen boolean
//                  std::cout << "peek north's south interface updated at x: " << mNX << " mY: " << mNY << " " << it->second->msnorth << " " << it->second->mssouth << " " << it->second->mseast << " " << it->second->mswest << std::endl;
//                  Cell * c =  new Cell(mCell_num, mNX, mNY, mZ, mCells[mNew_coordinates]->msnorth, mNorthInterface, mCells[mNew_coordinates]->mseast, mCells[mNew_coordinates]->mswest,mCells[mNew_coordinates]->north, true, mCells[mNew_coordinates]->east,mCells[mNew_coordinates]->west, mCells[mNew_coordinates]->visited);
                  mCells[mNew_coordinates]->mssouth = mNorthInterface;
                  mCells[mNew_coordinates]->south = true;
              }
      }
        //reaches this case if this is the first time peeking at this interface.  Ask if this interface has already been seen.  If not, issue a look command
      else if(mCells[mCoordinates]->north == false){
              mCells[mCoordinates]->north = true;
              gdir = true;
              deduct += .25;
              mCharge_num -= 0.25;
              return true;
      }
        //Reaches this case if this LOOK has previously been issued so don't need to waste another command.  Just need to make sure to
        //advance to the next interface and reset the current agent interface to this cell (Otherwise it would still be last cell's interface value)
      else {
          gIndex++;
          mNorthInterface = mCells[mCoordinates]->msnorth;
      }
        //False means that interface had previously been explored
        return false;
    }
    bool Swampert::LookSouth(const ai::Agent::Percept * percept) {
        if (gdir){
            gdir = false;
            gIndex = 2;
            ai::Agent::PerceptAtom a = percept->GetAtom("LOOK");
            mSouthInterface = a.GetValue().c_str();
            mNX = mX + 0;
            mNY = mY - 1000;
            mNew_coordinates = Coordinates(mNX, mNY);
            std::map<std::pair<int, int>, Cell*>::const_iterator it;
            it = this->mCells.find(mNew_coordinates);
            if(it == mCells.end())
            {
                if(mSouthInterface == "cliff"){
                  Cell * c =  new Cell(mCell_num, mNX, mNY, mZ, "wall", "", "", "", 1,0,0,0,0);
                  mCells[mNew_coordinates] = c;
                }
                else if(mSouthInterface == "wall"){
                  Cell * c =  new Cell(mCell_num, mNX, mNY, mZ, "cliff", "", "", "",1,0,0,0,0);
                  mCells[mNew_coordinates] = c;
                }
                else { Cell *c =  new Cell(mCell_num, mNX, mNY, mZ, mSouthInterface, "", "", "",1,0,0,0,0);
                mCells[mNew_coordinates] = c;
//                                  std::cout << "peek south interfaces unexplored new cell created and north interface set: " << " " << " " << mSouthInterface << " " << " " << " " << " " << std::endl;
                }
            }
            else
            {
//                                  std::cout << "peek south's north interface updated at x: " << mNX << " mY: " << mNY << " " << it->second->msnorth << " " << it->second->mssouth << " " << it->second->mseast << " " << it->second->mswest << std::endl;
                  Cell * c =  new Cell(mCell_num, mNX, mNY, mZ, mSouthInterface, mCells[mNew_coordinates]->mssouth, mCells[mNew_coordinates]->mseast, mCells[mNew_coordinates]->mswest, true, mCells[mNew_coordinates]->south, mCells[mNew_coordinates]->east,mCells[mNew_coordinates]->west, mCells[mNew_coordinates]->visited);
                  mCells[mNew_coordinates] = c;
              }
      }
      else if (mCells[mCoordinates]->south == false){
          mCells[mCoordinates]->south = true;
          gdir = true;
          deduct += .25;
          mCharge_num -= 0.25;
          return true;
      }
      else {
          gIndex++;
          mSouthInterface = mCells[mCoordinates]->mssouth;
      }
    return false;
    }
    bool Swampert::LookEast(const ai::Agent::Percept * percept) {
        if (gdir){
            gdir = false;
            gIndex = 3;
            ai::Agent::PerceptAtom a = percept->GetAtom("LOOK");
            mEastInterface = a.GetValue().c_str();
            mNX = mX + 1000;
            mNY = mY;
            mNew_coordinates = Coordinates(mNX,mNY);
            std::map<std::pair<int, int>, Cell*>::const_iterator it;
            it = this->mCells.find(mNew_coordinates);
            if(it == mCells.end())
            {
                if(mEastInterface == "cliff"){
                  Cell * c =  new Cell(mCell_num, mNX, mNY, mZ, "", "", "","wall", 0,0,0,1,0);
                  mCells[mNew_coordinates] = c;
                }
                else if(mEastInterface == "wall"){
                  Cell * c =  new Cell(mCell_num, mNX, mNY, mZ, "", "", "", "cliff",0,0,0,1,0);
                  mCells[mNew_coordinates] = c;
                }
                else { Cell * c =  new Cell(mCell_num, mNX, mNY, mZ, "", "", "", mEastInterface,0,0,0,1,0);
                  mCells[mNew_coordinates] = c;
//                                  std::cout << "west interfaces unexplored new cell created east interface set: " << " " << " " << " " << " " << " " << " " << mWestInterface << std::endl;
                }//                                  std::cout << "east interfaces unexplored new cell created west interface set: " << " " << " " << " " << " " << mEastInterface << " " << " " << std::endl;
            }
            else
            {
//                                  std::cout << "peek east's west interface updated at x: " << mNX << " mY: " << mNY << " " << it->second->msnorth << " " << it->second->mssouth << " " << it->second->mseast << " " << it->second->mswest << std::endl;
                  Cell * c =  new Cell(mCell_num, mNX, mNY, mZ, mCells[mNew_coordinates]->msnorth, mCells[mNew_coordinates]->mssouth, mCells[mNew_coordinates]->mseast, mEastInterface, mCells[mNew_coordinates]->north,mCells[mNew_coordinates]->south,mCells[mNew_coordinates]->east, true, mCells[mNew_coordinates]->visited);
                  mCells[mNew_coordinates] = c;
              }
      }
      else if (mCells[mCoordinates]->east == false){
          mCells[mCoordinates]->east = true;
          gdir = true;
          deduct += .25;
          mCharge_num -= 0.25;
          return true;
      }
      else {
          gIndex++;
          mEastInterface = mCells[mCoordinates]->mseast;
      }
    return false;
    }
    bool Swampert::LookWest(const ai::Agent::Percept * percept) {
        if (gdir){
            gdir = false;
            gIndex = 4;
            ai::Agent::PerceptAtom a = percept->GetAtom("LOOK");
            mWestInterface = a.GetValue().c_str();
            mNX = mX - 1000;
            mNY = mY;
            mNew_coordinates = Coordinates(mNX,mNY);
            std::map<std::pair<int, int>, Cell*>::const_iterator it;
            it = this->mCells.find(mNew_coordinates);
            if(it == mCells.end())
            {
                if(mWestInterface == "cliff"){
                  Cell * c =  new Cell(mCell_num, mNX, mNY, mZ, "", "", "wall", "",0,0,1,0,0);
                  mCells[mNew_coordinates] = c;
                }
                else if(mWestInterface == "wall"){
                  Cell * c =  new Cell(mCell_num, mNX, mNY, mZ, "", "", "cliff", "",0,0,1,0,0);
                  mCells[mNew_coordinates] = c;
                }
                else { Cell * c =  new Cell(mCell_num, mNX, mNY, mZ, "", "", mWestInterface, "",0,0,1,0,0);
                  mCells[mNew_coordinates] = c;
//                                  std::cout << "west interfaces unexplored new cell created east interface set: " << " " << " " << " " << " " << " " << " " << mWestInterface << std::endl;
                }
            }
            else
            {
//                                  std::cout << "peek west's east interface updated at x: " << mNX << " mY: " << mNY << " " << it->second->msnorth << " " << it->second->mssouth << " " << it->second->mseast << " " << it->second->mswest << std::endl;
                  Cell * c =  new Cell(mCell_num, mNX, mNY, mZ, mCells[mNew_coordinates]->msnorth,mCells[mNew_coordinates]->mssouth, mWestInterface, mCells[mNew_coordinates]->mswest,mCells[mNew_coordinates]->north,mCells[mNew_coordinates]->south, true, mCells[mNew_coordinates]->west,mCells[mNew_coordinates]->visited);
                  mCells[mNew_coordinates] = c;
              }
      }
      else if (mCells[mCoordinates]->west == false){
          mCells[mCoordinates]->west = true;
          gdir = true;
          deduct += .25;
          mCharge_num -= 0.25;
          return true;
      }
      else {
          gIndex++;
          mWestInterface = mCells[mCoordinates]->mswest;
      }
    return false;
    }
void Swampert::VisitCell(Cell * c, const ai::Agent::Percept *percept) {
        gIndex = 0;
                        lastsize = 0;

        examine = false;
        gObjectsIndex = 0;
        if(!skip){
        c =  new Cell(mCell_num, mX, mY, mZ, mNorthInterface, mSouthInterface, mEastInterface, mWestInterface, 1,1,1,1, true);
        if(!mCells[mCoordinates]->visited){
            std::cout << gCount <<" CELL NUMBER IS: " << c->mcell_num << " AND VISITING NEW CELL OBJECT PERCEPT SIZE = " << mObjectIds.size() << std::endl;
            mCell_num = gCount;
            gCount++;
            mCells[mCoordinates]->visited = true;
            mCells[mCoordinates] = c;
            std::cout << gCount <<" final cell interfaces at mCell: " << c->mcell_num <<  " mX: " << mX << " mY: " << mY << " " << "mZ: " << mZ << " " << mNorthInterface << " " << mSouthInterface << " " << mEastInterface << " " << mWestInterface << std::endl;
        }
        }
        mState->SetCell(*mCells[mCoordinates]);
        mState->SetX(mX);
        mState->SetY(mY);
        mState->SetZ(mZ);
        mState->SetCharge(mCharge_num);
        mState->SetCellNum(mCell_num);
        mState->SetHP(mHp_num);
        mModel = new Model(mGoalx, mGoaly, mGoalz, mBase_num, mCharge_num, mHp_num, mX, mY, mZ, true);
        mProblem = new Problem(mState, mModel, true, false);
//                    State * goal = new State();
        mProblem->SetGoal(startstate);
        mProblem->mCells = mCells;
        mProblemRecharge = new Problem(mState, mModel, false, true);
        mProblemRecharge->SetGoal(startstate);
        mProblemRecharge->mCells = mCells;
//        mProblem->SetRocks(1);
//        mProblemRecharge->SetRocks(1);
    }

    std::pair<int, int> Swampert::Coordinates(int x, int y){
        std::pair<int, int> coordinates;
        coordinates.first = x;
        coordinates.second = y;
        return coordinates;
    }
    Cell * Swampert::getCell(std::pair<int, int> pair){
        Cell * c =  new Cell();
        if(start){
            start = false;
            startstate->SetX(0);
            startstate->SetY(0);
            startstate->SetZ(0);
            std::cout << "NEW CELL AND START STATE" << std::endl;
            c =  new Cell(mCell_num, 0, 0, 0, "","","","",0,0,0,0,0);
            mCells[pair] = c;
        }
        return mCells[pair];
    }

    std::map<std::string, std::string> Swampert::ParseObject(std::string object) {
            std::map<std::string, std::string> parsed_object;
            std::string objid ="";
        std::string color ="";
        std::string shape ="";
        std::string size ="";
        std::string size2 ="";
        std::vector<std::pair<std::string, int>> vec;
        std::pair<std::string, int> pair;
        pair.first = "";
        pair.second = 1;
        //these are the vectors representing the possible origins
        vec.push_back(pair);
        vec.push_back(pair);
        vec.push_back(pair);
        //IF NEW ORIGIN
        //vec.push_back(pair);
        bool c = 0,s = 0,si = 0,si2 = 0,obj = 0;
        for (unsigned int i = 0; object[i] != '\0'; i++){
            if (isspace(object[i]) && !obj){
                obj = true;
//                                std::cout << objid << std::endl;
                continue;
            }
            else if (!obj) objid += object[i];
            if (isspace(object[i]) && !c && obj){
                c = true;
                std::map<std::string, std::vector<std::pair<std::string, int>>>::const_iterator it;
                it = mColors.find(color);
                if(it == mColors.end())
                {
                    mColors[color] = vec;
                }
//                                std::cout << color << " = color: " << mColors[color] << std::endl;
                continue;
            }
            else if (obj && !c) color += object[i];
            if (isspace(object[i]) && !s){
                s  = true;
                std::map<std::string, std::vector<std::pair<std::string, int>>>::const_iterator it;
                it = mShapes.find(shape);
                if(it == mShapes.end())
                {
                    mShapes[shape] = vec ;
                }
//                                std::cout << shape << " = shape: " << mShapes[shape].second << std::endl;
                continue;
            }
            else if (c && !s) shape += object[i];
            if (isspace(object[i]) && !si){
                si  = true;
                std::map<std::string, std::vector<std::pair<std::string, int>>>::const_iterator it;
                it = mSizes.find(size);
                if(it == mSizes.end())
                {
                    mSizes[size] = vec;
                }
//                                std::cout << size << " = size: " << mSizes[size].second << std::endl;
                continue;
            }
            else if (c && s && !si) size += object[i];
            if (isspace(object[i]) && si && !si2){
                si2  = true;
                std::map<std::string, std::vector<std::pair<std::string, int>>>::const_iterator it;
                it = mSizes2.find(size2);
                if(it == mSizes2.end())
                {
                    mSizes2[size2] = vec;
                }
//                                std::cout << size2 << " = size2: " << mSizes2[size2].second << std::endl;
                continue;
            }
            else if (c && s && si && !si2) size2 += object[i];
        }
        parsed_object["COLOR"] = color;
        parsed_object["SHAPE"] = shape;
        parsed_object["SIZE"] = size;
        parsed_object["SIZE2"] = size2;
        parsed_object["ID"] = objid;
        parsed_object["OBJECT"] = color+ " " + shape + " " + size + " " + size2;
//                        std::cout << "THIS IS THE OBJECT -----------------------> " << parsed_object["OBJECT"] << std::endl;
        return parsed_object;
        }

void Swampert::GetObjectCounts(std::map<std::string, std::string> parsed_object, std::string val) {
        std::string object = parsed_object["OBJECT"];
        std::string size2 = parsed_object["SIZE2"];
        std::string size = parsed_object["SIZE"];
        std::string shape = parsed_object["SHAPE"];
        std::string color = parsed_object["COLOR"];
        checkDepositValue = false;
        std::vector<std::pair<std::string, int>> vec;
        std::pair<std::string, int> pair;
        pair.first = "";
        pair.second = 1;
        vec.push_back(pair);
        vec.push_back(pair);
        vec.push_back(pair);
        //IF A NEW ORIGIN
        //vec.push_back(pair);
        std::map<std::string, std::vector<std::pair<std::string, int>>>::const_iterator it;
        it = mLearnedObjects.find(object);
        if(it == mLearnedObjects.end())
        {
            mLearnedObjects[object] = vec;
            std::cout << "A NEW OBJECT IS BEING LEARNED = " << val << " OBJECT is: " << object << std::endl;
        }
    //                        std::cout << "VALUE = " << val << " OBJECT is: " << object <<  std::endl;
        if(val == "50"){
            std::pair<std::string, int> p;
            p.first = "NATIVE";
            p.second = mSizes2[size2][0].second + 1;
            std::pair<std::string, int> p1;
            p1.first = "NATIVE";
            p1.second = mSizes[size][0].second + 1;
            std::pair<std::string, int> p2;
            p2.first = "NATIVE";
            p2.second = mShapes[shape][0].second + 1;
            std::pair<std::string, int> p3;
            p3.first = "NATIVE";
            p3.second = mColors[color][0].second + 1;
            mSizes2[size2][0] = p;
            mSizes[size][0] = p1;
            mShapes[shape][0] = p2;
            mColors[color][0] = p3;
            mLearnedObjects[object][0].first = "NATIVE";
            mLearnedObjects[object][0].second += 1;
            mOriginTable[0] += 1;
    //                            mValuableDropoffIds.push_back(dropoffIndex-1);
        }
        else if(val == "10"){
            std::pair<std::string, int> p;
            p.first = "MIMIC";
            p.second = mSizes2[size2][1].second + 1;
            std::pair<std::string, int> p1;
            p1.first = "MIMIC";
            p1.second = mSizes[size][1].second + 1;
            std::pair<std::string, int> p2;
            p2.first = "MIMIC";
            p2.second = mShapes[shape][1].second + 1;
            std::pair<std::string, int> p3;
            p3.first = "MIMIC";
            p3.second = mColors[color][1].second + 1;
            mSizes2[size2][1] = p;
            mSizes[size][1] = p1;
            mShapes[shape][1] = p2;
            mColors[color][1] = p3;
            mLearnedObjects[object][1].first = "MIMIC";
            mLearnedObjects[object][1].second += 1;
            mOriginTable[1] += 1;
    //                            mValuableDropoffIds.push_back(dropoffIndex-1);
        }
        //IF NEW ORIGIN
//        else if(val == "-??"){
//            std::pair<std::string, int> p;
//            p.first = "????";
//            p.second = mSizes2[size2][2].second + 1;
//            std::pair<std::string, int> p1;
//            p1.first = "????";
//            p1.second = mSizes[size][2].second + 1;
//            std::pair<std::string, int> p2;
//            p2.first = "????";
//            p2.second = mShapes[shape][2].second + 1;
//            std::pair<std::string, int> p3;
//            p3.first = "????";
//            p3.second = mColors[color][2].second + 1;
//            mSizes2[size2][?] = p;
//            mSizes[size][?] = p1;
//            mShapes[shape][?] = p2;
//            mColors[color][?] = p3;
//            mLearnedObjects[object][?].first = "????";
//            mLearnedObjects[object][?].second += 1;
//        mOriginTable[?] += 1;
//        }
        else if(val == "-50"){
            std::pair<std::string, int> p;
            p.first = "EARTH";
            p.second = mSizes2[size2][2].second + 1;
            std::pair<std::string, int> p1;
            p1.first = "EARTH";
            p1.second = mSizes[size][2].second + 1;
            std::pair<std::string, int> p2;
            p2.first = "EARTH";
            p2.second = mShapes[shape][2].second + 1;
            std::pair<std::string, int> p3;
            p3.first = "EARTH";
            p3.second = mColors[color][2].second + 1;
            mSizes2[size2][2] = p;
            mSizes[size][2] = p1;
            mShapes[shape][2] = p2;
            mColors[color][2] = p3;
            mLearnedObjects[object][2].first = "EARTH";
            mLearnedObjects[object][2].second += 1;
            mOriginTable[2] += 1;
        }
        else std::cout << " IT SEEMS THIS OBJECT WAS PREVIOUSLY DEPOSITED.  POSSIBLY IN THE FIRST 100.  SKIP IT. " << val << "=value " << std::endl;
}

std::string Swampert::GetObjectValue(const ai::Agent::Percept * percept) {
     ai::Agent::PerceptAtom a = percept->GetAtom("DEPOSIT_VALUE");
    std::string value = a.GetValue().c_str();
//    std::cout << "VALUE AT PERCEPT = " << value << std::endl;
    bool obj1 = true;
    std::string val = "";
    for (unsigned int i = 0; value[i] != '\0'; i++){
        if(value[i] == '.') break;
        if(isspace(value[i])){
            obj1 = false;
            continue;
        }
        if(!obj1){
            val += value[i];
        }
    }
    return val;
 }
 void Swampert::PrintQuit(std::string message){
    std::map<std::string, std::vector<std::pair<std::string, int>>>::const_iterator it;
    std::cout << "THIS IS THE COUNT OF AN OBJECTS ORIGIN  " << std::endl;
    for (it = mLearnedObjects.begin(); it != mLearnedObjects.end(); it++)
    {
        std::cout << "OBJECT: " << it->first << " NATIVE: " << it->second[0].second <<" MIMIC: " << it->second[1].second <<" EARTH: " << it->second[2].second /*<< IF NEW ORIGIN "????: " << it->second[?].second */<< std::endl<< std::endl;
    }
    std::cout << "THESE ARE THE COUNTS MAPPING VALUES TO ORIGINS" << std::endl;
    for (it = mColors.begin(); it != mColors.end(); it++)
    {
        std::cout << "COLOR: " << it->first << " NATIVE: " << mColors[it->first][0].second<< " MIMIC: " << mColors[it->first][1].second<< " EARTH: " << mColors[it->first][2].second /*<< IF NEW ORIGIN "????: " << it->second[?].second */<< std::endl<< std::endl;
    }
    for (it = mShapes.begin(); it != mShapes.end(); it++)
    {
        std::cout << "SHAPE: " << it->first << " NATIVE: " << mShapes[it->first][0].second<< " MIMIC: " << mShapes[it->first][1].second<< " EARTH: " << mShapes[it->first][2].second /*<< IF NEW ORIGIN "????: " << it->second[?].second */<< std::endl<< std::endl;
    }
    for (it = mSizes.begin(); it != mSizes.end(); it++)
    {
        std::cout << "SIZE: " << it->first << " NATIVE: " << mSizes[it->first][0].second<< " MIMIC: " << mSizes[it->first][1].second<< " EARTH: " << mSizes[it->first][2].second /*<< IF NEW ORIGIN "????: " << it->second[?].second */<< std::endl<< std::endl;
    }
    std::cout <<"THERE WERE a total of " << gtotaleverything << " objects and " << gTotalDeposits << " were deposited " << std::endl;
    std::cout << "Agent is safely quitting... "+message << std::endl;
 }
    void Swampert::parsePercepts(const ai::Agent::Percept * percept, bool oldcell){
        std::stringstream ss;
        ss.str(percept->GetAtom("CHARGE").GetValue());
        ss.clear();
        ss >> mCharge_num;
        ss.str(percept->GetAtom("X_LOC").GetValue());
        ss.clear();
        ss >> mX;
        ss.str(percept->GetAtom("Y_LOC").GetValue());
        ss.clear();
        ss >> mY;
        ss.str(percept->GetAtom("Z_LOC").GetValue());
        ss.clear();
        ss >> mZ;
        startstate->SetX(basex);startstate->SetY(basey);startstate->SetZ(basez);
        ss.str(percept->GetAtom("HP").GetValue());
        ss.clear();
        ss >> mHp_num;
        if(!oldcell){
            mObjectIds.clear();
            for(unsigned int i = 0; i < percept->NumAtom(); i++)
            {
                std::string object_id;
                ai::Agent::PerceptAtom a = percept->GetAtom(i);
                if(std::strncmp(a.GetName().c_str(), "OBJECT_", 7) == 0)
                {
                    for(int j = 7; a.GetName().c_str()[j] != '\0'; j++){
                        object_id += a.GetName().c_str()[j];
                    }
                }
                if(object_id != "") {
                    mObjectIds.push_back(object_id);
//                    std::cout << " these are the object ids: " << object_id << std::endl;
                }
            }
            mObjectIds.push_back("end");
        }
    }
    ai::Agent::Action * Swampert::Program(const ai::Agent::Percept * percept)
    {
        ai::Scavenger::Action *action = new ai::Scavenger::Action;
        if(mCharge_num < 0.5 && !need_charge && mX == 0 && mY == 0){
            std::cout << "RECHARGE ACTION ACTUAL CHARGE IS: "<< mCharge_num << std::endl;
            action->SetCode(ai::Scavenger::Action::RECHARGE);
            mCharge_num += 25.0;
            pickupcheck = false;
            return action;
        }
//        std::cout << mCharge_num << std::endl;
        if(need_charge/* || (first100 && mX == 0 && mY == 0)*/){
            if(gTotalDeposits == 1000){
                            std::cout << "REACHED MAXIMUM DEPOSITS OF 1000 AND NOW WILL QUIT...." << std::endl;
                            action->SetCode(ai::Scavenger::Action::QUIT);
                            return action;
            }
            parsePercepts(percept, true);
            mCoordinates = Coordinates(mX,mY);
                while(((need_charge && mCharge_num < 100.00)/* || (first100 && mX == 0 && mY == 0)*/) && (dropoffIndex+100) < (int)mDropoffObjectIds.size()){
//                    std::cout << dropoffIndex << " HELLLLLLLLOOOOOOO     " << first100index << " " << gTotalDeposits << std::endl;
                    if(checkDepositValue){
//                        std::cout << " THE return object is : " << mDropoffObjectIds[dropoffIndex]+" "+mObjects[mDropoffObjectIds[dropoffIndex]] + " " + val << std::endl;
                        checkDepositValue = false;
                        std::string val = GetObjectValue(percept);
                        std::map<std::string, std::map<std::string, std::string>>::const_iterator it;
                        if(first100index < 100){
                            it = mParsedObjects.find(mDropoffObjectIds[first100randomindex]);
                        }
                        else it = mParsedObjects.find(mDropoffObjectIds[dropoffIndex]);
                        //std::map<std::string, std::string> parsed_object = it.second;   //ParseObject(mDropoffObjectIds[dropoffIndex]+" "+mObjects[mDropoffObjectIds[dropoffIndex]]);
                        GetObjectCounts(it->second, val);
                        if(first100index > 99){ std::cout << " THE ACTUAL VALUE WAS: " << val << " AND THE NUMBER OF DEPOSITS THUS FAR IS: " << gTotalDeposits << std::endl; dropoffIndex++; mDropoffObjectIds[first100randomindex] = gSEEN; }
                        else first100index++;
//                        mParsedObjects.erase(mDropoffObjectIds[dropoffIndex]);
                        gTotalDeposits++;
                        continue;
                    }
                    else if(end && needDropOff && (dropoffIndex+100) < (int)mDropoffObjectIds.size() && mCharge_num > 1){
//                        if(!first100){
                        // MAKE SURE THAT THE RANDOM INDEX HASN'T BEEN USED PREVIOUSLY
                        if(mDropoffObjectIds[dropoffIndex] == gSEEN) {dropoffIndex++; continue;}
                        if(first100index < 100){
                            int num = mDropoffObjectIds.size();
                            first100randomindex = rand() % num;
                            bool again = 0;
                            for(unsigned int i = 0; i < randomnums.size(); i++){
                                while(randomnums[i] == first100randomindex){
//                                    std::cout << randomnums[i] << " " << first100randomindex << std::endl;
                                    first100randomindex = rand() % num;
                                    if(randomnums[i] != first100randomindex){ again = true; break; }
                                }
                                if(again) {i = 0; again = false; }
                            }
                            randomnums.push_back(first100randomindex);
                            action->SetObjectId(mDropoffObjectIds[first100randomindex]);
                            action->SetCode(ai::Scavenger::Action::DEPOSIT);
                            mCharge_num -= 0.25;
                            checkDepositValue = true;
                            return action;
                        }
                        std::map<std::string, std::map<std::string, std::string>>::const_iterator it;
                        it = mParsedObjects.find(mDropoffObjectIds[dropoffIndex]);
                        std::map<std::string, std::string> parsed_object;
                        parsed_object = it->second;
                        std::string color = parsed_object["COLOR"];
                        std::string shape = parsed_object["SHAPE"];
                        std::string size = parsed_object["SIZE"];
                        std::vector<double> probs = ProbOriginGivenColorShapeSize(size, color, shape);
                        double origintotal = mOriginTable[0] + mOriginTable[1] + mOriginTable[2];
                        double ev = probs[0] * mOriginTable[0]/origintotal;
                        double ev1  = probs[1] * mOriginTable[1]/origintotal;
                        double ev2 = probs[2] * mOriginTable[2]/origintotal;
                        std::cout << "THESE ARE ESTIMATED VALUES: " << ev << " " << ev1 << " " << ev2 << std::endl << " THESE ARE THE PROBABILITIES OF EACH ORIGIN: " << probs[0] << " " << probs[1] << " " << probs[2] << " AND EV: " << ev << std::endl;
//                        if(probs[0] > probs[2] || probs[1] > probs[2]){
                        if(ev > ev2 || ev1 > ev2){
                            action->SetObjectId(mDropoffObjectIds[dropoffIndex]);
                            action->SetCode(ai::Scavenger::Action::DEPOSIT);
                            mCharge_num -= 0.25;
                            checkDepositValue = true;
                            return action;
                        }
                        else{
                            dropoffIndex++;
                            continue;
                        }
//                        action->SetObjectId(mDropoffObjectIds[dropoffIndex]);
//                        action->SetCode(ai::Scavenger::Action::DEPOSIT);
//                        mCharge_num -= 0.25;
//                        checkDepositValue = true;
//                        return action;
//                                }
                            }
                    std::cout << "RECHARGE ACTION DURING DEPOSITS ACTUAL CHARGE IS: "<< mCharge_num << std::endl;
                    action->SetCode(ai::Scavenger::Action::RECHARGE);
                    return action;
                }
                while(need_charge && mCharge_num < 100.00){
                    std::cout << "RECHARGE ACTION DURING DEPOSITS ACTUAL CHARGE IS: "<< mCharge_num << std::endl;
                    action->SetCode(ai::Scavenger::Action::RECHARGE);
                    return action;
                }
                std::cout << dropoffIndex << " ITS GETTING HERE BUT QHY? " << " " << mDropoffObjectIds.size() << std::endl;
                if(end && need_charge && ((dropoffIndex+100) >= 1000 || (dropoffIndex+100) >= (int)mDropoffObjectIds.size() - 1)){
                    PrintQuit("NO MORE CELLS TO VISIT OR REACHED MAX DEPOSITS");
                    action->SetCode(ai::Scavenger::Action::QUIT);
                    return action;
                }
//                semi_known_obj = false;
//                mDropoffObjectIds.clear();
                needDropOff = false;
//                first100 = false;
//                dropoffIndex = 0;
                need_charge = false;
        }

        if (mPath.empty()){
        if(!gdir){
        if(!examine) {
            parsePercepts(percept, false);
            mState = new State();
            mModel = new Model();
            mProblem = new Problem(mState, mModel, true, false);
            mProblemRecharge = new Problem(mState, mModel, false, true);
            mCoordinates = Coordinates(mX,mY);
            examine = true;
            pickupcheck = false;
        }
        Cell *c = getCell(mCoordinates);
        while(mObjectIds[gObjectsIndex] != "end"){
            std::map<std::string, std::string>::const_iterator it;
            if(lastsize > 4 && mCharge_num - MARGIN - lastBacktoBaseCost < 0.5 && !(mX == 0 && mY == 0)){
                std::cout << " RUNNING OUT OF CHARGE DURING EXAMINE PHASE!!!!!! " << mCharge_num << " = CHARGE && # OF DEPOSITS BEFORE HEADING BACK IS: " << lastsize << std::endl;
                lastsize = 0;
                pickupcheck = false;
                examine = false;
                gIndex = 4;
                exploreMode = true;
                skip = true;
//                semi_known_obj = false;
                break;
            }
            if(!c->visited && !pickupcheck){
//                if(first100index < 100){
////                    std::cout << mObjectIds.size() << " size of percept objects " << first100index << std::endl;
//                    int num = mObjectIds.size() - 1;
//                    first100randomindex = rand() % num;
//                    action->SetObjectId(mObjectIds[first100randomindex]);
//                    action->SetCode(ai::Scavenger::Action::EXAMINE);
//                    mCharge_num-= 0.25;
//                    pickupcheck = true;
//                    mObjects[mObjectIds[first100randomindex]] = "";
//                    gtotaleverything++;
//                    return action;
//                }
                gtotaleverything++;
                action->SetObjectId(mObjectIds[gObjectsIndex]);
                action->SetCode(ai::Scavenger::Action::EXAMINE);
                mObjects[mObjectIds[gObjectsIndex]] = "";
                pickupcheck = true;
                deduct += .5;
                mCharge_num -= 0.25;
                return action;
            }
//            if(first100index == 99) first100 = true;
            if(!c->visited && pickupcheck){
//                if(first100index < 100){
//                    ai::Agent::PerceptAtom a = percept->GetAtom("EXAMINE");
//                    std::string object = a.GetValue().c_str();
//                    std::map<std::string, std::string> parsed_object = ParseObject(object);
//                    mParsedObjects[mObjectIds[first100randomindex]] = parsed_object;
//                    mObjects[mObjectIds[first100randomindex]] = parsed_object["OBJECT"];
////                    std::cout << "FIRST100 pickup id? " << mObjectIds[first100randomindex] << std::endl;
////                std::cout << "IS IT GETTING TO THE NORMAL LEARNING EXAMINE CALL? ID = " << mObjectIds[first100randomindex] << " " << mObjects[mObjectIds[first100randomindex]] << " SHOULD BE EMPTY " << std::endl;
//                    action->SetObjectId(mObjectIds[first100randomindex]);
//                    action->SetCode(ai::Scavenger::Action::PICKUP);
////                    std::cout << "IS IT PICKING UP THE SAME OBJECT? OBJECT IN PICKUP = " << parsed_object["OBJECT"] << "ID = " << mObjectIds[first100randomindex] <<  " " << mObjects[parsed_object["ID"]] << std::endl;
//                    mCharge_num -= 0.25;
//                    needDropOff = true;
//                    pickupcheck = false;
//                    gTotalCount++;
//                    first100index++;
//                    mDropoffObjectIds.push_back(parsed_object["ID"]);
//                    return action;
//                }
                //do all the checking at dropoff time
                ai::Agent::PerceptAtom a = percept->GetAtom("EXAMINE");
                std::string object = a.GetValue().c_str();
                std::map<std::string, std::string> parsed_object = ParseObject(object);
                mParsedObjects[mObjectIds[gObjectsIndex]] = parsed_object;
                mObjects[mObjectIds[gObjectsIndex]] = parsed_object["OBJECT"];
                if(object == ""){
                    std::cout << "THIS OBJECT WAS EMPTY FOR SOME REASON. SKIP IT. size = "<< mObjectIds.size() << " " << object << " " << mObjectIds[gObjectsIndex] << std::endl;
                    gObjectsIndex++;
                    continue;
                }
//                std::cout << "IS IT GETTING TO THE NORMAL LEARNING PICKJP CHECK CALL? object = " << mObjects[mObjectIds[gObjectsIndex]] << " " << mObjectIds[gObjectsIndex] << std::endl;
//                std::map<std::string, std::vector<std::pair<std::string, int>>>::const_iterator it;
//                it = mLearnedObjects.find(parsed_object["OBJECT"]);
//                if(it != mLearnedObjects.end())
//                {
////                    std::cout << "OBJECT HAS BEEN LEARNED SO I SHOULD KNOW ITS VALUE " << std::endl;
//                    int high = mLearnedObjects[parsed_object["OBJECT"]][0].second;
//                    int med = mLearnedObjects[parsed_object["OBJECT"]][1].second;
//                    int low = mLearnedObjects[parsed_object["OBJECT"]][2].second;
////                    std::cout << mObjects[mObjectIds[gObjectsIndex]] << "=========== SHALL I PICK THIS UP ? Object " << high << " " << med << " " << low <<std::endl;
////                    std::cout << mObjects[mObjectIds[gObjectsIndex]] << high << " " << med << " " << low << "*****************KNOWN **Object=================="<<std::endl;
//    //                    if((high + med > low)){
//                        if((high >= low ||  med >= low)){
//    //                                        std::cout << mObjects[mObjectIds[gObjectsIndex]] << high << " " << med << " " << low << "*****************KNOWN **ObjectPICKED UP*********************"<<std::endl;
//                            action->SetObjectId(mObjectIds[gObjectsIndex]);
//                            action->SetCode(ai::Scavenger::Action::PICKUP);
//                            mCharge_num-= 0.25;
//                            needDropOff = true;
//                            pickupcheck = false;
//                            mDropoffObjectIds.push_back(parsed_object["ID"]);
//                            gObjectsIndex++;
//                            gTotalCount++;
//                            return action;
//                        }
//                        else{
//    //                                        std::cout << mObjects[mObjectIds[gObjectsIndex]] << high << " " << med << " " << low << "KNOWN OBJECT DIDN'T MEET CRITERIA"<<std::endl;
//                            pickupcheck = false;
//                            gObjectsIndex++;
//                            continue;
//                        }
//                 }
//                    std::map<std::string, std::vector<std::pair<std::string, int>>>::const_iterator cit;
//                    std::map<std::string, std::vector<std::pair<std::string, int>>>::const_iterator sit;
//                    it = mColors.find(parsed_object["COLOR"]);
//                    cit = mShapes.find(parsed_object["SHAPE"]);
//                    sit = mSizes.find(parsed_object["SIZE"]);
//                    if((it != mColors.end() || cit !=  mShapes.end() || sit != mSizes.end())){
//////////                        std::cout << "********************************IS IT GEETING HERE*********************************** " << std::endl;
//                        int high = mColors[parsed_object["COLOR"]][0].second;
//                        int med = mColors[parsed_object["COLOR"]][1].second;
//                        int low = mColors[parsed_object["COLOR"]][2].second;
//                        bool first = high + med >= low;
//                        int high2 = mShapes[parsed_object["SHAPE"]][0].second;
//                        int med2 = mShapes[parsed_object["SHAPE"]][1].second;
//                        int low2 = mShapes[parsed_object["SHAPE"]][2].second;
//                        bool second = high2 + med2 >= low2;
//                        int high3 = mSizes[parsed_object["SIZE"]][0].second;
//                        int med3 = mSizes[parsed_object["SIZE"]][1].second;
//                        int low3 = mSizes[parsed_object["SIZE"]][2].second;
//                        bool third = high3 + med3 >= low3;
//                        if((first && second) || (first && third) || (second && third)){
//                        std::cout << mObjects[mObjectIds[gObjectsIndex]] << high << " " << med << " " << low << " " << high2 <<  " " << med2 << " " << low2 << " " << high3 << " " << med3 << " " << low3<< "=========== PICKED UP SEMI-KNOWN **Object=================="<<std::endl;
    //                        while(mDropoffObjectIds[dropoffIndex] == "-1") dropoffIndex++;
    //                        std::cout << "ITEMS DEPOSITED" << std::endl;
                                    mDropoffObjectIds.push_back(mObjectIds[gObjectsIndex]);
                                    action->SetObjectId(mObjectIds[gObjectsIndex]);
                                    action->SetCode(ai::Scavenger::Action::PICKUP);
                                    mCharge_num-= 0.25;
                                    needDropOff = true;
                                    pickupcheck = false;
                                    gObjectsIndex++;
//                                    gTotalCount++;
//                                    semi_known_obj = true;
//                                    std::cout << "CHARGE AT PICKUP : " << mCharge_num << std::endl;
                                    lastsize++;
                                    return action;
                }

          pickupcheck = false;
          gObjectsIndex++;
          }
       }
        Cell * c =  getCell(mCoordinates);
//        if(first100index >= 1000) exploreMode = false;
//            need_charge = true;
        if(gIndex == 0) {
              bool look = LookNorth(percept);
              if(look) {
                  action->SetDirection(ai::Scavenger::Location::Direction::NORTH);
                  action->SetCode(ai::Scavenger::Action::LOOK);
//                  std::cout << "CHARGE = " << mCharge_num << " \nACTION: " << action->GetCode() << std::endl;
                  return action;
              }
          }
          if(gIndex == 1) {
              bool look = LookSouth(percept);
              if(look) {
                  action->SetDirection(ai::Scavenger::Location::Direction::SOUTH);
                  action->SetCode(ai::Scavenger::Action::LOOK);
//                  std::cout << "CHARGE = " << mCharge_num << " \nACTION: " << action->GetCode() << std::endl;
                  return action;
              }
          }
          if(gIndex == 2) {
              bool look = LookEast(percept);
              if(look) {
                  action->SetDirection(ai::Scavenger::Location::Direction::EAST);
                  action->SetCode(ai::Scavenger::Action::LOOK);
//                  std::cout << "CHARGE = " << mCharge_num << " \nACTION: " << action->GetCode() << std::endl;
                  return action;
              }
          }
          if(gIndex == 3) {
              bool look = LookWest(percept);
              if(look) {
                  action->SetDirection(ai::Scavenger::Location::Direction::WEST);
                  action->SetCode(ai::Scavenger::Action::LOOK);
//                  std::cout << "CHARGE = " << mCharge_num << " \nACTION: " << action->GetCode() << std::endl;
                  return action;
              }
          }
          if(gIndex == 4){
            VisitCell(c, percept);
          }
        }

        if(exploreMode){
            exploreMode = false;
//            ai::Search::Frontier *fringe  = new ai::Search::DFFrontier;
////            ai::Search::Frontier *fringe  = new ai::Search::BFFrontier;
            ai::Search::Frontier *fringe  = new ai::Search::UCFrontier;
            ai::Search::Graph *search = new ai::Search::Graph(mProblem, fringe);
//            ai::Search::Tree *search = new ai::Search::Tree(mProblem, fringe);
            if (mPath.empty())
            {

                if(search->Search()){
                    std::list<ai::Search::Node *> *solution = search->GetSolution().GetList();
                    std::list<ai::Search::Node *>::const_iterator it;
                    UCSpathCost = solution->back()->GetPathCost();
                    if(MARGIN + UCSpathCost + lastBacktoBaseCost > 100.0){
                        PrintQuit("PATH TO NEXT CELL WON'T ALLOW FOR A SUCCESSFUL RETURN"/*+".....TURNING ON EXHAUSTION MODE"*/);
//                        mExhaustionMode = 1;
                    }
                    if(gTotalDeposits > 1000){
                            std::cout << "REACHED MAXIMUM DEPOSITS OF 1000 AND NOW WILL QUIT...." << std::endl;
                            end = true;
                        }
                    for(it = solution->begin(); it != solution->end(); it++)
                    {
                        if((*it)->GetAction())
                        {
                            (*it)->GetAction()->Display();
                        }
                    }
                }
                else {
                    std::cout << "NO PATH FOUND TO A NEW CELL TRYING ROCKS...." << std::endl;
                    ai::Search::Frontier *fringe  = new ai::Search::UCFrontier;
                    mProblem->SetRocks(1);
                    ai::Search::Graph *search = new ai::Search::Graph(mProblem, fringe);
//                    end = true;
                        if(search->Search()){
                        std::list<ai::Search::Node *> *solution = search->GetSolution().GetList();
                        std::list<ai::Search::Node *>::const_iterator it;
                        UCSpathCost = solution->back()->GetPathCost();
                        if(gTotalDeposits > 1000){
                            std::cout << "REACHED MAXIMUM DEPOSITS OF 1000 AND NOW WILL QUIT...." << std::endl;
                            end = true;
                        }
                        else if(MARGIN + UCSpathCost + lastBacktoBaseCost > 100.0){
                            PrintQuit("PATH TO NEXT CELL WON'T ALLOW FOR A SUCCESSFUL RETURN"/*+".....TURNING ON EXHAUSTION MODE"*/);
//                            mExhaustionMode = 1;
                        }
                        for(it = solution->begin(); it != solution->end(); it++)
                        {
                            if((*it)->GetAction())
                            {
                                (*it)->GetAction()->Display();
                            }
                        }
                        }

                        else {
                            std::cout << "NO PATH FOUND...." << std::endl;
                            end = true;
                        }
                }
                ai::Search::Frontier *recharge_fringe  = new ai::Search::AStarFrontier;
                ai::Search::Graph *recharge_search = new ai::Search::Graph(mProblemRecharge, recharge_fringe);
                if(recharge_search->Search()){
                             std::list<ai::Search::Node *> *solution = recharge_search->GetSolution().GetList();
                            lastBacktoBaseCost = solution->back()->GetPathCost() + UCSpathCost;
                            double pathome = solution->back()->GetPathCost();
                            if(pathome < 0.001) pathome = UCSpathCost;
                            std::cout << "SROPOFF SIZE ============ " << mDropoffObjectIds.size() << std::endl;
                             std::cout << "UCS PATH COST ============ " << UCSpathCost << std::endl;
                             std::cout << "HOME PATH COST ============ " << pathome << std::endl;
                             std::cout << "HP ============ " << mHp_num << std::endl;
//                             std::cout << "RECHARGE ============ " << mCharge_num-deduct << std::endl;
                             std::cout << "Charge: " << mCharge_num << " Est. CHARGE to get home if I go to a new cell first: " << MARGIN + pathome + UCSpathCost - (mZ/1000.0) << std::endl;
                             if(MARGIN + pathome + UCSpathCost - (mZ/1000.0) > mCharge_num - .5|| end || skip){
                                 gohome = true;
                                 skip = false;
                                 if(!mExhaustionMode){
                                     while(!mPath.empty()){
                                         mPath.pop();
                                     }
                                     std::list<ai::Search::Node *> *solution = recharge_search->GetSolution().GetList();
                                     std::list<ai::Search::Node *>::const_iterator it;
                                    for(it = solution->begin(); it != solution->end(); it++)
                                    {
                                        if((*it)->GetAction())
                                        {
                                            (*it)->GetAction()->Display();
                                        }
                                    }
                                 }

                            }
                }
                else {
                    std::cout << "SHOULD ALWAYS FIND A PATH BACK HOME...TRYING ROCKS" << std::endl;
                    mProblemRecharge->SetRocks(1);
                    ai::Search::Frontier *recharge_fringe  = new ai::Search::AStarFrontier;
                    ai::Search::Graph *recharge_search = new ai::Search::Graph(mProblemRecharge, recharge_fringe);
                    if(recharge_search->Search()){
                             std::list<ai::Search::Node *> *solution = recharge_search->GetSolution().GetList();
                            lastBacktoBaseCost = solution->back()->GetPathCost() + UCSpathCost;
                            double pathome = solution->back()->GetPathCost();
                            if(pathome < 0.001) pathome = UCSpathCost;
                             std::cout << "UCS PATH COST ============ " << UCSpathCost << std::endl;
                             std::cout << "HOME PATH COST ============ " << pathome << std::endl;
                             std::cout << "HP ============ " << mHp_num << std::endl;
//                             std::cout << "RECHARGE ============ " << mCharge_num-deduct << std::endl;
                             std::cout << "Charge: " << mCharge_num << " Est. CHARGE to get home if I go to a new cell first: " << MARGIN + pathome + UCSpathCost - (mZ/1000.0) << std::endl;
                             if(MARGIN + pathome + UCSpathCost - (mZ/1000.0) > mCharge_num - .5|| end || skip || mDropoffObjectIds.size() > 1000){
                                 gohome = true;
                                 skip = false;
                                 if(!mExhaustionMode){
                                     while(!mPath.empty()){
                                         mPath.pop();
                                     }
                                     std::list<ai::Search::Node *> *solution = recharge_search->GetSolution().GetList();
                                     std::list<ai::Search::Node *>::const_iterator it;
                                    for(it = solution->begin(); it != solution->end(); it++)
                                    {
                                        if((*it)->GetAction())
                                        {
                                            (*it)->GetAction()->Display();
                                        }
                                    }
                                 }

                            }
                    }
                    else {
                        std::cout << "STILL DIDNt FIND A PATH BACK HOME...something went wrong" << std::endl;
                    }

                }
            }
                    deduct = 0;
         }
        if(!mPath.empty() || (gohome && mPath.empty()))
        {
                    std::string act = "";
                    if(!(gohome && mPath.empty())){
                        act = mPath.front();
                        mPath.pop();
                    }
                    if (mPath.empty()) {
                        exploreMode = true;
                        if(gohome){
                            gohome = false;
                            need_charge = true;
                        }
                    }
                    if(act == "GO_NORTH")
                    {
                        action->SetCode(ai::Scavenger::Action::GO_NORTH);
                    }
                    else if(act == "GO_SOUTH")
                    {
                        action->SetCode(ai::Scavenger::Action::GO_SOUTH);
                    }
                    else if(act == "GO_WEST")
                    {
                        action->SetCode(ai::Scavenger::Action::GO_WEST);
                    }
                    else if(act == "GO_EAST")
                    {
                        action->SetCode(ai::Scavenger::Action::GO_EAST);
                    }
                    else if(act == "QUIT")
                    {
                        action->SetCode(ai::Scavenger::Action::QUIT);
                    }
                    else if (act == "GO_HOME") act = act;
                    else std::cout << "SHOULDN'T GET HERE AT ALL????" << std::endl;
//                std::cout << "CHARGE = " << mCharge_num << " \nACTION MOVES: " << action->GetCode() << std::endl;
                return action;
                }
        else {
            PrintQuit("NO PATH WAS FOUND");
            action->SetCode((int)ai::Scavenger::Action::QUIT);
        }
     return action;
    }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////





    Action::Action()
    {
      /* empty */
    }

    Action::Action(const Action &rhs)
    {
      *this = rhs;
    }

    Action::~Action()
    {
      /* empty */
    }

    Action &Action::operator=(const Action &rhs)
    {
      this->type   = rhs.type;
      return *this;
    }

    bool Action::operator==(const Action &rhs) const
    {
      return (this->type == rhs.type);
    }
    void Action::Display() const
    {
        std::string act;
      switch(this->type)
        {
        case GO_NORTH:
//          std::cout << "N" << std::endl;
          mPath.push("GO_NORTH");
          break;
        case GO_SOUTH:
//          std::cout << "S" << std::endl;
          mPath.push("GO_SOUTH");
          break;
        case GO_EAST:
//          std::cout << "E" << std::endl;
          mPath.push("GO_EAST");
          break;
        case GO_WEST:
//          std::cout << "W" << std::endl;
          mPath.push("GO_WEST");
          break;
        case QUIT:
          std::cout << "Quit: " << std::endl;
          mPath.push("QUIT");
          break;
        default:
          std::cout << "Unknown action" << std::endl;
          break;
        }
    }

    bool Action::SetType(int type_in)
    {
      if(type_in >= GO_NORTH && type_in <= QUIT)
        {
          this->type = type_in;
          return true;
        }
      return false;
    }

    int Action::GetType() const
    {
      return this->type;
    }
  }
}
