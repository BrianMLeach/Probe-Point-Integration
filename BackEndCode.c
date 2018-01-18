/**************************************************************************************************/
/* CONFIDENTIAL HELLA  KGaA HUECK & CO. (HKG)                                                     */
/*                                                                                                */
/* This is an unpublished work of authorship, which contains trade secrets, created in 2005.      */
/* Hella KGaA Hueck & Co. owns all rights to this work and intends to maintain it in confidence   */
/* to preserve its trade secret status. Hella KGaA Hueck & Co. reserves the right, under the      */
/* copyright laws of Germany or those of any other country that may have jurisdiction, to protect */
/* this work as an unpublished work, in the event of an inadvertent or deliberate unauthorized    */
/* publication. Hella KGaA Hueck & Co. also reserves its rights under all copyright laws to       */
/* protect this work as a published work, when appropriate. Those having access to this work may  */
/* not copy it, use it, modify it or disclose the information contained in it without the written */
/* authorization of Hella KGaA Hueck & Co.                                                        */
/*                                                                                                */
/**************************************************************************************************/
/* Public | Private | API Function                                                                */
/*                                                                                                */
/* Project     : EPS_PP                                                                           */
/*                                                                                                */
/* Module      : BackEndCode                                                                      */
/*                                                                                                */
/* File name   : BackEndCode.c                                                                    */
/*                                                                                                */
/* Company     : Hella Engineering France                                                         */
/* Department  : SW                                                                               */
/* Author      : Name of the author                                                               */
/*                                                                                                */
/* Limitations : None.                                                                            */
/* Constraints : The software shall, at least, be compliant with the GHS Compiler.                */
/*                                                                                                */
/* Description of the module                                                                      */
/* Description of the functionality of the file                                                   */
/* Implementation of the module                                                                   */
/**************************************************************************************************/

/**************************************************************************************************/
/**                      QAC exceptions                                                           */
/**************************************************************************************************/
/* PRQA S 838,839 EOF *//* QAC rule violation due to AUTOSAR requirement for memory mapping (memap.h). */

/**************************************************************************************************/
/**                      Include Section                                                          */
/**************************************************************************************************/
/* RULE PROG_048 : Each module shall include its own header file. */
#include "BackEndCode.h"


unsigned long rdtsc () 
{
	//return *(unsigned long*)0xffe0A004 ;
	return *(unsigned long*)0xFFDD8004;
}

periodicity_struct periodicity_data [ MAXPERIODICITYINDEX ] = {0};

void PP_PERIODICITY_INITIALIZE (unsigned int index, unsigned int expected)
{
    periodicity_struct * _this ;
    if (index < MAXPERIODICITYINDEX )
    {
        _this = & periodicity_data [index] ;  
        _this -> periodicity_arrays [0] = 0 ; /* number of calls */
        _this -> periodicity_arrays [1] = 0 ; 
        _this -> periodicity_arrays [2] = 0 ;  
        _this -> periodicity_arrays [3] = 0 ;
        _this -> periodicity_arrays [4] = 0 ;
        _this -> periodicity_arrays [5] = 0 ;   
        _this -> periodicity_average = 0;
        _this -> periodicity_previous = 0;
        _this -> periodicity_call_counter = 0;  
        _this -> periodicity_expected = expected ;     
    }
}


void PP_PERIODICITY_CHECK (unsigned int index)
{
    unsigned int sixteenth ;
    unsigned int eighth ;
    unsigned int min ;
    unsigned int max ;
    unsigned int happymin ;
    unsigned int happymax ;
    unsigned int i ;
    unsigned int diff ;
    periodicity_struct * _this ;
    if (index < MAXPERIODICITYINDEX )
    {
        _this = & periodicity_data [index] ;
        _this -> periodicity_arrays [0]++ ; /* one more call */
        min = _this -> periodicity_expected - (_this -> periodicity_expected >>3) ;
        max = _this -> periodicity_expected + (_this -> periodicity_expected >>3) ;
        happymin = _this -> periodicity_expected - (_this -> periodicity_expected >>4) ;
        happymax = _this -> periodicity_expected + (_this -> periodicity_expected >>4) ;        
        if (_this -> periodicity_previous != 0) 
        {
            diff = rdtsc() - _this -> periodicity_previous ;
            if ( diff < min || diff > max )
            {
                _this -> periodicity_arrays [1]++ ;       
            } 
            else if ( diff < happymin || diff > happymax )
            {
                _this -> periodicity_arrays [2]++ ;       
            } 
            else 
            {
                _this -> periodicity_arrays [3]++ ;       
            } 
            if ( _this -> periodicity_arrays [4] == 0 )
            {
                _this -> periodicity_arrays [4] = (periodicity_small_t ) 0xffffffff ;
            }
            /* lose low order 8 bits of the time to fit inside 16 bits, assumes time is always less than 16m ticks */
            diff >>= 8 ;
            if (diff < _this -> periodicity_arrays [4] )
            {
                _this -> periodicity_arrays [4] = diff ; /* minimum diff seen */
            }
            if (diff > _this -> periodicity_arrays [5] )
            {
                _this -> periodicity_arrays [5] = diff ; /* maximum diff seen */
            }
            _this -> periodicity_average += diff ; /* counting values shifted right by 8 */
        }
        _this -> periodicity_call_counter++ ;
        _this -> periodicity_previous = rdtsc();
    }
}

periodicity_struct * PP_GET_PERIODICITY_REPORT (unsigned int index)
{
    periodicity_struct * _this = 0;
    if (index < MAXPERIODICITYINDEX )
    {
        _this = & periodicity_data [index] ;   
    }
    return _this ;
} 


//Function will return a pointer to saved result data[c,p1,p2,p3]  
const VCAST_PP_FuncSaveParms_Data_T * VCAST_PP_FuncGetParms (unsigned int index)
{
    unsigned char datastoreindex ;
    VCAST_PP_FuncSaveParms_Data_T * retval = 0;
    if (index < VCAST_PP_FUNCSAVEPARMS_ENABLE_SIZE)
    {
        datastoreindex = vcast_pp_funcsaveparms_enable [index];
        if (datastoreindex > 0 && datastoreindex <= VCAST_PP_FUNCSAVEPARMS_DATA_SIZE)
        {
            retval = VCAST_PP_FuncSaveParms_Data + (datastoreindex-1) ;
        }
    }
    return retval;
}

/*******************************************************************************
//Function Description : This Function will collect stored parameters data
//for the probe point
// 
//Input Parameters : 
//Index  - Holds the index numbber where stored data index is saved
//P1,P2 - Parameters 
//
//Return: Pointer to stored data index 
*******************************************************************************/


void VCAST_PP_FuncSaveParms_2 (unsigned int index, int p1, int p2)
{
    unsigned char datastoreindex ;
    VCAST_PP_FuncSaveParms_Data_T * pdata ;
    if (index < VCAST_PP_FUNCSAVEPARMS_ENABLE_SIZE) // max size is 100
    {
        datastoreindex = vcast_pp_funcsaveparms_enable [index]; //max size is 10
        if (datastoreindex > 0 && datastoreindex <= VCAST_PP_FUNCSAVEPARMS_DATA_SIZE)//max size is 10
        {
            pdata = VCAST_PP_FuncSaveParms_Data + (datastoreindex-1);
            pdata -> counter++ ;
            pdata -> p1 = p1 ;
            pdata -> p2 = p2 ;
            pdata -> p3 = 0 ;
        }
    }
    return ;
}

//function will check if requested probe point could be enabled 
//if database(10 array of [c,p1,p2,p3]) has any space
//return True(1) if probe point is enabled else false(0)
int VCAST_PP_FuncGetParms_Enable (unsigned int index, unsigned int datastoreindex )
{
    int retval = 0;
    VCAST_PP_FuncSaveParms_Data_T * pdata ;
    if (index < VCAST_PP_FUNCSAVEPARMS_ENABLE_SIZE)//max size is 100
    {
        if (datastoreindex < VCAST_PP_FUNCSAVEPARMS_DATA_SIZE)//max size is 10
        {
            /* save the value 1-10 not 0-9 */
            /* because 0 is a value meaning disabled */
            vcast_pp_funcsaveparms_enable [index] = (datastoreindex+1); //3 is saved in 10th index
            pdata = VCAST_PP_FuncSaveParms_Data + datastoreindex; //address of VCAST_PP_FuncSaveParms_Data[2] 
            pdata -> counter = 0;		//clear all initially
            pdata -> p1 = 0 ;
            pdata -> p2 = 0 ;
            pdata -> p3 = 0 ;
            retval = TRUE;	//bikash added TRUE
        }
		else				//bikash
			retval = FALSE; //bikash
    }
    return retval ;
}


int Mini_TestCase()
{
	const VCAST_PP_FuncSaveParms_Data_T * P_Data;
	int retVal =0;
	periodicity_struct * ps;
	//call enable func
	VCAST_PP_FuncGetParms_Enable(10,2);
	IoHwAbstr_SetDiscrete(7,1);
	//wait here
	P_Data=VCAST_PP_FuncGetParms (10);
	if(P_Data!=0)
	{
		retVal = P_Data->counter;
		retVal<<=16;
		retVal|=((unsigned char) P_Data->p1);
		retVal<<=8;
		retVal|=((unsigned char) P_Data->p2);
		
	}
	if (periodicity_check_enabled==0)
	{
		periodicity_check_enabled = 1;
        PP_PERIODICITY_INITIALIZE (1, 32768); /* we have no idea what 1ms looks like in hw ticks */
	}
	else
	{
		ps = PP_GET_PERIODICITY_REPORT (1);
	}

	return retVal;
}





/**************************************************************************************************/
/**                     End of File                                                               */
/**************************************************************************************************//* PRQA S 862 *//* QAC rule violation due to AUTOSAR requirement for memory mapping (memap.h). */
