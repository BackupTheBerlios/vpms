/*
  VPMS - Penna Model Simulation
  
  Copyleft 2009: <Mikolaj Sitarz> sitarz@gmail.com

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation,  version 3
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
   
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "vpms.hh"
#include "model.hh"
#include "guile.hh"
#include "config.hh"
#include "MersenneTwister.h"

using namespace std;

parameters p;
MTRand random;
Config cfg;

Environment *env = NULL;

void initialize() {

  p.N = cfg[N];
  p.B = cfg[B];
  p.R = cfg[R];
  p.M = cfg[M];
  p.P = cfg[P];
  p.T = cfg[T];
  p.initGenome = cfg[initGenome];
  random.seed(cfg[rndSeed]);
}

int main(int argc, char **argv) {

  init_guile(argc, argv);
  return 0;

}
