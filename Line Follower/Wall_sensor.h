// David Sigurdsson // dabbis@gmail.com 
// This code may not be used by anyone unless otherwise agreed upon.

/////////////////////////////////////////////////////////////////////////
//                       Function prototypes:        	               //
/////////////////////////////////////////////////////////////////////////

//Initializing the ADC
void InitADC(void);

//Get the average values from the sensors
int get_ADC_Value(int sensor);

void showPlace(uint8_t value);

void turn_90(uint8_t direction);

void forward_distance(uint16_t distance, uint8_t speed);

void forwardWallLeftStop(void);

void parallelWall(void);

void go(signed int dist, uint8_t dir, uint8_t ref);

void forward_stop(uint16_t distance);

void runWall(void);

void compare_sensors_test(void);

float calcWallError(void);

