#include <EEPROM.h>

// This platform doesn't support true double precision math
#define HAL_NO_DOUBLE_PRECISION

// This is for ~16MHz AVR processors or similar.  These MCUs can't do a wide range of degrees for misalignment modeling (Goto Assist.)
// They can, however, run Motor Timer ISR's w/stepper driver signal timing met in one pass vs. two for faster processors
#define HAL_SLOW_PROCESSOR

// Enable a pseudo low priority level for Timer1 (sidereal clock) so the
// critical motor ISR's don't get delayed by the big slow sidereal clock ISR
#define HAL_USE_NOBLOCK_FOR_TIMER1

// Fast port writing help
#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))
#define TGL(x,y) (x^=(1<<y))

extern long int siderealInterval;
extern void SiderealClockSetInterval (long int);

// Initialize the timer that handles the sidereal clock
void HAL_Init_Timer_Sidereal() {
  SiderealClockSetInterval(siderealInterval);
}

// Initialize Axis1 and Axis2 motor timers and set their priorities

// Note the granularity below, these run at 0.5uS per tick with 16bit depth
#define HAL_FIXED_PRESCALE_16BIT_MOTOR_TIMERS

void HAL_Init_Timers_Motor() {
  // ~0 to 0.032 seconds (31 steps per second minimum, granularity of timer is 0.5uS) /8  pre-scaler
  TCCR3B = (1 << WGM12) | (1 << CS11);
  TCCR3A = 0;
  TIMSK3 = (1 << OCIE3A);

  // ~0 to 0.032 seconds (31 steps per second minimum, granularity of timer is 0.5uS) /8  pre-scaler
  TCCR4B = (1 << WGM12) | (1 << CS11);
  TCCR4A = 0;
  TIMSK4 = (1 << OCIE4A);
}

// Set timer1 to interval (in microseconds*16), this is the 1/100 second sidereal timer
void Timer1SetInterval(long iv, double rateRatio) {
  iv=round(((double)iv)/rateRatio);

  TCCR1B = 0; TCCR1A = 0;
  TIMSK1 = 0;

  // set compare match register to desired timer count:
  if (iv<65536) { TCCR1B |= (1 << CS10); } else {
  iv=iv/8;
  if (iv<65536) { TCCR1B |= (1 << CS11); } else {
  iv=iv/8;
  if (iv<65536) { TCCR1B |= (1 << CS10); TCCR1B |= (1 << CS11); } else {
  iv=iv/4;  
  if (iv<65536) { TCCR1B |= (1 << CS12); } else {
  iv=iv/4;
  if (iv<65536) { TCCR1B |= (1 << CS10); TCCR1B |= (1 << CS12); 
  }}}}}
  
  OCR1A = iv-1;
  // CTC mode
  TCCR1B |= (1 << WGM12);
  // timer compare interrupt enable
  TIMSK1 |= (1 << OCIE1A);
}

#define QuickSetIntervalAxis1(r) (OCR3A=r)
#define QuickSetIntervalAxis2(r) (OCR4A=r)

//void QuickSetIntervalAxis1(uint32_t r) { OCR3A=r; }
//void QuickSetIntervalAxis2(uint32_t r) { OCR4A=r; }

// --------------------------------------------------------------------------------------------------
// We use standard #define's to do **fast** digitalWrite's to the step and dir pins for the Axis1/2 stepper drivers
#define StepPinAxis1_HIGH SET(Axis1StepPORT, Axis1StepBit)
#define StepPinAxis1_LOW CLR(Axis1StepPORT, Axis1StepBit)
#define DirPinAxis1_HIGH SET(Axis1DirPORT, Axis1DirBit)
#define DirPinAxis1_LOW CLR(Axis1DirPORT, Axis1DirBit)

#define StepPinAxis2_HIGH SET(Axis2StepPORT, Axis2StepBit)
#define StepPinAxis2_LOW CLR(Axis2StepPORT, Axis2StepBit)
#define DirPinAxis2_HIGH SET(Axis2DirPORT, Axis2DirBit)
#define DirPinAxis2_LOW CLR(Axis2DirPORT, Axis2DirBit)


