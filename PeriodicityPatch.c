#define TIMER_RANGE 0x100000

unsigned long now = rdtsc();
if(now > _this -> periodicity_previous)
  diff = now - _this -> periodicity_previous; /* No wrap */
else
  diff = TIMER_RANGE - (now - _this -> periodicity_previous); /* Wrap */