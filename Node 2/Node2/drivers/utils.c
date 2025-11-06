#include <stdint.h>


uint8_t PID(uint8_t inn, uint8_t ref, uint8_t Kp, uint8_t Ki, uint8_t Kd,uint8_t *out){

//Max limits for integral
const int16_t MAX_windup = 1000;
const int16_t MIN_windup = -1000;

//Max limits for output
const int16_t MAX_out = 1000;
const int32_t MIN_out = -1000;

const uint8_t dt = 1; //assuming constant timestep between measurments

//These values should carry between calls
static uint8_t prev_inn = 0;
static uint32_t integral = 0;


//Find current error
int16_t error = (int16_t)ref - (int16_t)inn;

//Update current integral
integral += error*dt;

// Anti windup for integral
if (integral> MAX_windup){
    integral = MAX_windup;
}else if(integral < MIN_windup){
    integral = MIN_windup;
}

//Calculate output
int32_t outvalue = Kp*error+Ki*integral-Kd*(inn-prev_inn)/dt;

//Limit output
if (outvalue > MAX_out){
    outvalue = MAX_out;
}else if(outvalue < MIN_out){
    outvalue = MIN_out;
}

prev_inn = inn;
*out = outvalue;

return 0;
}


// uint8_t Kalman(state){}