
#include "string.h"

#define BUFFERSIZE 100
char vcast_ascii_coverage_data_pool[BUFFERSIZE];

int vcast_ascii_coverage_data_pos = 0;

void vcast_trigger_breakpoint() 
{
    int i ;
    for (i=0; i < BUFFERSIZE; i++)
    {
        vcast_ascii_coverage_data_pool [i] = 0 ;
    }
    vcast_ascii_coverage_data_pos = 0 ;
}

void vcast_custom_std_output_external (const char * S, int flush)
{
    int len = strlen(S);
    if (( len + vcast_ascii_coverage_data_pos) >= (BUFFERSIZE-2) )
    {
        vcast_trigger_breakpoint() ;
    }
    if ( ( len + vcast_ascii_coverage_data_pos) < (BUFFERSIZE-2) ) /* this should be true unless the string is too long */
    {
        while (*S)
        {
            vcast_ascii_coverage_data_pool [vcast_ascii_coverage_data_pos++] = *S++;
        }
        vcast_ascii_coverage_data_pool [vcast_ascii_coverage_data_pos++] = '\n';
        vcast_ascii_coverage_data_pool [vcast_ascii_coverage_data_pos] = 0;
    }
}