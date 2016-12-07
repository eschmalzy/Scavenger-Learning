/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "model.h"

namespace ems
{
  namespace Scavenger
  {
      
      Model::Model(){}
      Model::Model(double gx,double gy,double gz,int base, double charge, double hp, double ax, double ay, double az, bool v)
        :mgoalx(gx), mgoaly(gy), mgoalz(gz), mbase_num(base), mcharge_num(charge), mhp_num(hp), magentx(ax), magenty(ay), magentz(az), visited(v)
      {
         //pass
      }
  }
}