static int32_t get_power_limited_torque(int32_t pedal_torque) {
  static int32_t tCAP = 0; // keeps track of calculated tMAX
  static int32_t count = 0; // keeps track of ramp
  
  if(speed < 0) { // check car is moving forward
    
    int32_t tMAX = power_limit*6000*10/((0-mc_readings.speed)*628); // calculate tMAX based on power limit and wheel speed
    if(tMAX > MAX_TORQUE) tMAX = MAX_TORQUE; // cap at max torque
    tCAP = tMAX;
    
    if(pedal_torque >= tCAP && count!= 5) { // ramp is over, get new tMAX
      if(count != 5) count++; return tCAP*count*100/500;
      
      if(count == 5) return tCAP; //ramp has been reached but pedal is still higher than tCAP
 
    } 
    count = 0; // else pedal is < tCAP, and so reset, and return pedal
    tCAP = 0;
    return pedal_torque;

  } else { // else car is not moving, reset and return pedal
    count = 0;
    tCAP = 0;
    return pedal_torque;
  }
}

static int32_t tCAP = 0;
  static int32_t count = 0;
  
  if(speed < 0) { // check car is moving forward
    
    if(count < 5 && count != 0) { // ramp is not over yet

      if(pedal_torque >= tMAX) { // check if car pedal is above allowed torque
        count++; // increase counter
        return tCAP*count*100/500; // return tCAP in incrementing fifths
      }
      count = 0; // else, reset the counter and return pedal
      tCAP = 0;
      return pedal_torque;
    } else { // else ramp is over, get new tMAX
      
      int32_t tMAX = power_limit*6000*10/((0-mc_readings.speed)*628); // calculate tMAX based on power limit and wheel speed
      if(tMAX > MAX_TORQUE) tMAX = MAX_TORQUE; // cap at max torque
      tCAP = tMAX;
      count++;
      return tCAP/5;
    }
    
    return pedal_torque;

  } else { // else car is not moving, reset and return pedal
    count = 0;
    tCAP = 0;
    return pedal_torque;
  }