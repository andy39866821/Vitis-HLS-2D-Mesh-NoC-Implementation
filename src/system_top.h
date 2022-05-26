#ifndef __SYSTEM_TOP_H__
#define __SYSTEM_TOP_H__

#include "define.h" 

extern "C" { 
    void system_top(
        flit_t input_flit_array[PORT_COUNT][SYSTEM_TOP_MEM_SIZE],
        flit_t output_flit_array[PORT_COUNT][SYSTEM_TOP_MEM_SIZE],
        int output_flit_count[PORT_COUNT],
        int pattern_num,
        int* cycle_count
    );
}


#endif