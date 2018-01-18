#include "Rte_Type.h"


void TestManagerMainFunction(void);
void TM_Pause(void);
void TMSetResult(boolean);

extern uint8 TM_GroupNumber;
extern uint8 TM_CaseNumber;
extern uint8 TM_Step;
extern boolean TM_AutoExec;

#ifndef PP_BACK_END_H
#define PP_BACK_END_H

/* max number of probe points of this type in the code */
#define VCAST_PP_FUNCSAVEPARMS_ENABLE_SIZE 100

/* max number of simultaneously active probe points */
#define VCAST_PP_FUNCSAVEPARMS_DATA_SIZE 10

#define MAXPERIODICITYINDEX 10


/* need to repeat these once per each "cycle counter" and index them with the index parameter*/
/* 6 counters, total calls, calls with time accurate to 1/16th. calls accurate to one eighth, others, min, max */
typedef unsigned short periodicity_small_t ;
typedef unsigned long periodicity_big_t ; /* as big as the clock */
typedef struct {
  periodicity_small_t periodicity_arrays [6] ;
  periodicity_big_t  periodicity_average ;
  periodicity_big_t  periodicity_previous ;
  periodicity_big_t  periodicity_expected ;  
  periodicity_small_t periodicity_call_counter ;
} periodicity_struct ;

periodicity_struct * PP_GET_PERIODICITY_REPORT (unsigned int index);

void PP_PERIODICITY_CHECK (unsigned int index);
void PP_PERIODICITY_INITIALIZE (unsigned int index, unsigned int expected);
void PP_SAVE_FUNC_CALL_ORDER (const char * text);
char * PP_GET_FUNC_CALL_ORDER ();


void VCAST_PP_FuncSaveParms_2 (unsigned int index, int p1, int p2);
/* the probe point might be called multiple times BUT we save only the last value */
typedef struct {
    int counter ;
    int p1 ;
    int p2 ;
    int p3 ;
} VCAST_PP_FuncSaveParms_Data_T;
const VCAST_PP_FuncSaveParms_Data_T * VCAST_PP_FuncGetParms (unsigned int index);
int VCAST_PP_FuncGetParms_Enable (unsigned int index, unsigned int datastoreindex );

VCAST_PP_FuncSaveParms_Data_T VCAST_PP_FuncSaveParms_Data [VCAST_PP_FUNCSAVEPARMS_DATA_SIZE];


/* values 1-10 indicate an acceptable slot 0-9 in the database */
unsigned char vcast_pp_funcsaveparms_enable [VCAST_PP_FUNCSAVEPARMS_ENABLE_SIZE] = {0};


int periodicity_check_enabled = 0;
#endif
