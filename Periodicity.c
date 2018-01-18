
#include "Periodicity.h"


unsigned long rdtsc () 
{
	//return *(unsigned long*)0xffe0A004 ;
	return *(unsigned long*)0xFFDD8004;
}

periodicity_struct periodicity_data [ MAXPERIODICITYINDEX ] = {0};

//Index - The index for the periodicity test (0 - MAXPERIODICITYINDEX).
//Expected - The change in the hardware counter over the expected period.
//Max_variance_percent. The maximum allowed variance in percent.
//						Maximum value is 20;

int PP_PERIODICITY_INITIALIZE (unsigned int index, PERIODICITY_VALUE expected,
							   unsigned int max_variance_percent)
{
    periodicity_struct * _this;
	int nRet = 0;
    if (index < MAXPERIODICITYINDEX)
    {
        _this = periodicity_data + index;
        _this->number_of_calls = 0;
        _this->variance_exceeded = 0;
        _this->variance_ok = 0;  
        _this->minimum = PERIODICITY_VALUE_MAXIMUM;
        _this->maximum = 0;
        _this->intn_average = 0;
        _this->intn_previous = 0;
		//Below 200 there is a lack of resolution. Also avoid computation overflow.
		if((expected >= 200) && (expected <= (PERIODICITY_VALUE_MAXIMUM / 20)))
		{
			PERIODICITY_VALUE variance = (expected * 20 / max_variance_percent) / 100;
			_this->intn_lower_bound = expected - variance;
			_this->intn_upper_bound = expected + variance;
			nRet = 1;
		}
    }
	return nRet;
}


void PP_PERIODICITY_CHECK (unsigned int index)
{
    PERIODICITY_VALUE diff;
    periodicity_struct *_this;

    if(index < MAXPERIODICITYINDEX)
    {
        _this = periodicity_data + index;
        if(_this->number_of_calls)
        {
			PERIODICITY_VALUE now = rdtsc();
            diff = now - _this->intn_previous;
            if((diff < _this->lower_bound) || (diff > _this->upper_bound))
                _this->variance_exceeded++;       
			else
				_this->variance_ok++; 
  
            if(diff < _this->minimum)
                _this->minimum = diff; /* minimum diff seen */
            if(diff > _this->maximum)
                _this->maximum = diff; /* maximum diff seen */

            _this->intn_average += (PERIODICITY_ACCUMULATE)diff;
			_this->intn_previous = now;
        }
		else
			_this->intn_previous = rdtsc();
        _this->number_of_calls++;
    }
}

periodicity_struct * PP_GET_PERIODICITY_REPORT (unsigned int index)
{
    periodicity_struct *_this;
    if (index < MAXPERIODICITYINDEX)
	{
		_this = periodicity_data + index;
		_this->average = _this->intn_average / (PERIODICITY_ACCUMULATE) _this->number_of_calls;
	}
	else
		_this = (periodicity_struct *) 0;
     return _this ;
} 

/**************************************************************************************************/
/**                     End of File                                                               */
/**************************************************************************************************
//* PRQA S 862 *//* QAC rule violation due to AUTOSAR requirement for memory mapping (memap.h). */
