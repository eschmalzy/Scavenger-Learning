/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   cell.h
 * Author: bstone
 *
 * Created on October 20, 2016, 9:47 AM
 */

#ifndef CELL_H
#define CELL_H
#include <string>

namespace ems
{
  namespace Scavenger
  {
      
class Cell
      {
      public:
          Cell();
          Cell(int cnum, double cx, double cy, double cz, std::string n, std::string s, std::string e, std::string w, bool north, bool south, bool east, bool west, bool visited);
          double  mcell_num;
        double mcell_x;
        double mcell_y;
        double mcell_z;
        std::string msnorth = "";
        std::string mssouth = "";
        std::string mseast = "";
        std::string mswest = "";
        bool north, south, east, west, visited;
        protected:
        private:
      };
  }
}

#endif /* CELL_H */

