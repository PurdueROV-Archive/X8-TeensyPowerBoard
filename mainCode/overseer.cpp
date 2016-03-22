// INCLUDES:
#include "overseer.h"


// CLASS FUNCTIONS:

// CONSTRUCTOR:
Overseer::Overseer(void)
{
	//GREAT CODE
	int i;
	flag_NewData = NEW_DATA;
	for (i = 0; i < 8; i++)
		thrusters.powers[i] = 0;
	for (i = 0; i < 8; i++)
		thrusters.currents[i] = 0;
	for (i = 0; i < 8; i++)
		thrusters.voltages[i] = 0;
	for (i = 0; i < 8; i++)
		thrusters.enabled[i] = 0;
  target_force = vect6Make(0,0,0,0,0,0);
  flag_NewData = 0;
  is_Overflowing = 0;
}


// Gives new data to Overseer, and sets NEW_DATA_FLAG to NEW_DATA
//
// @note - Run in the interrupt. Does no calculations.
void Overseer::update(vect6 force, vect3 pivotPos, char on_off)
{
	target_force = force;
	thrustMapper.adjustPivotPosition(pivotPos);
	thrustMapper.changeMapperMatrix(on_off);

	flag_NewData = NEW_DATA;
}

// Checks if new data has been logged (NEW_DATA_FLAG set), calculates if true.
//
// @returns - NEW_DATA_FLAG.
// @note - Run in main while loop (not during interrupt).
int Overseer::checkForUpdate(void)
{
	if (flag_NewData == NEW_DATA)
  {
    flag_NewData = NO_NEW_DATA;
		calculateAndPush();
  }
	return flag_NewData;
}

// Calculates the thrust for each motor, and pushes to the motors.
void Overseer::calculateAndPush(void)
{
	thrustMapper.calculateThrustMap(target_force);
    int max = 0;
    if ((max = max8(thrustMapper.thrust_map)) > THRUST_MAX)
        scaleOverflow(&thrustMapper.thrust_map, max);
    else
        is_Overflowing = 0;
	// send the thrustMapper.thrust_map to the motors (thrusters) here:
}


// Scales all thrust values by the maximum overflowing thrust, keeping the same force vector.
void Overseer::scaleOverflow(vect8 * thrust_map, int32_t max)
{
    if (max < THRUST_MAX)
        return;

    is_Overflowing = 1;
    float scale = (float) THRUST_MAX / max;
    scale *= FLOATPT_TO_INT_SCALE;

    thrust_map->a *= scale;
    thrust_map->b *= scale;
    thrust_map->c *= scale;
    thrust_map->d *= scale;
    thrust_map->e *= scale;
    thrust_map->f *= scale;
    thrust_map->g *= scale;
    thrust_map->h *= scale;

    thrust_map->a /= (int32_t)FLOATPT_TO_INT_SCALE;
    thrust_map->b /= (int32_t)FLOATPT_TO_INT_SCALE;
    thrust_map->c /= (int32_t)FLOATPT_TO_INT_SCALE;
    thrust_map->d /= (int32_t)FLOATPT_TO_INT_SCALE;
    thrust_map->e /= (int32_t)FLOATPT_TO_INT_SCALE;
    thrust_map->f /= (int32_t)FLOATPT_TO_INT_SCALE;
    thrust_map->g /= (int32_t)FLOATPT_TO_INT_SCALE;
    thrust_map->h /= (int32_t)FLOATPT_TO_INT_SCALE;
}


// Will update information about the motors.
void Overseer::updateFromThrusters(void)
{
	// Update Current
	// Update Voltage
}

void Overseer::doRamping(void)
{
    //do any motor ramping which needs to occur, here
    if (Tset.L.x > Tcurrent.L.x) {
        Tcurrent.L.x += MIN(1024,Tset.L.x - Tcurrent.L.x);
    } else if(Tset.L.x < Tcurrent.L.x){
        Tcurrent.L.x += MAX(-1024,Tset.L.x - Tcurrent.L.x);
    }
    
    if (Tset.L.y > Tcurrent.L.y) {
        Tcurrent.L.y += MIN(1024,Tset.L.y - Tcurrent.L.y);
    } else if(Tset.L.y < Tcurrent.L.y){
        Tcurrent.L.y += MAX(-1024,Tset.L.y - Tcurrent.L.y);
    }
    
    if (Tset.L.z > Tcurrent.L.z) {
        Tcurrent.L.z += MIN(1024,Tset.L.z - Tcurrent.L.z);
    } else if(Tset.L.z < Tcurrent.L.z){
        Tcurrent.L.z += MAX(-1024,Tset.L.z - Tcurrent.L.z);
    }
    
    /////
    
    if (Tset.R.x > Tcurrent.R.x) {
        Tcurrent.R.x += MIN(1024,Tset.R.x - Tcurrent.R.x);
    } else if(Tset.R.x < Tcurrent.R.x){
        Tcurrent.R.x += MAX(-1024,Tset.R.x - Tcurrent.R.x);
    }
    
    if (Tset.R.y > Tcurrent.R.y) {
        Tcurrent.R.y += MIN(1024,Tset.R.y - Tcurrent.R.y);
    } else if(Tset.R.y < Tcurrent.R.y){
        Tcurrent.R.y += MAX(-1024,Tset.R.y - Tcurrent.R.y);
    }
    
    if (Tset.R.z > Tcurrent.R.z) {
        Tcurrent.R.z += MIN(1024,Tset.R.z - Tcurrent.R.z);
    } else if(Tset.R.z < Tcurrent.R.z){
        Tcurrent.R.z += MAX(-1024,Tset.R.z - Tcurrent.R.z);
    }
    
    //pseudo code
    //if Tset > T
        //T = T + min(1024,residual)
    //else
        //T = T - min(1024,residual)
    
    //send through CAN to motors
    
}


// FOR DEBUGGING USE ONLY!!!
vect8 Overseer::getThrust_Map(void)
{
  return thrustMapper.thrust_map;
}

ThrustMapper Overseer::getThrustMapper(void)
{
  return thrustMapper;
}

vect6 Overseer::getTargetForce(void)
{
  return target_force;
}

int Overseer::areOverseerAndMapperCommunicating(void)
{
  int i = 0;

  i = (target_force.L.x == thrustMapper.getCurrentForceVector().L.x && target_force.L.y == thrustMapper.getCurrentForceVector().L.y && target_force.L.z == thrustMapper.getCurrentForceVector().L.z && target_force.R.x == thrustMapper.getCurrentForceVector().R.x && target_force.R.y == thrustMapper.getCurrentForceVector().R.y && target_force.R.z == thrustMapper.getCurrentForceVector().R.z) ? 1 : 0;
  
  return i;
}

