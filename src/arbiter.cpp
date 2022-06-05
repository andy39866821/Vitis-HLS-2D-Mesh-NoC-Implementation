#include "arbiter.h"



arbiter::arbiter() {

}

void arbiter::update_combinational(
    bool output_VC_full[PORT_COUNT],
    bool request[PORT_COUNT][PORT_COUNT],   // Input
    bool grant[PORT_COUNT][PORT_COUNT],     // Output
    arbiter_internal_wire* output_wire
){
    #pragma HLS INLINE
    // Update flit counter
    for(int i = 0 ; i < PORT_COUNT ; i++){
        #pragma HLS UNROLL
        if(request[RR_counter[i]][i] == true)
            output_wire->flit_counter_next[i] = (flit_counter[i] == FLITS_PER_PACKET-1 ? 0 : flit_counter[i]+1);
        else
            output_wire->flit_counter_next[i] = flit_counter[i];
    }

    // Update Round-Robin counter
    for(int i = 0 ; i < PORT_COUNT ; i++){
        #pragma HLS UNROLL
        if(flit_counter[i] == FLITS_PER_PACKET-1)   // send FLITS_PER_PACKET flits then change priority
            output_wire->RR_counter_next[i] = (RR_counter[i] == PORT_COUNT-1 ? 0 : RR_counter[i]+1);
        else if(request[RR_counter[i]][i] == false && flit_counter[i] == 0) // have priority but no request
            output_wire->RR_counter_next[i] = (RR_counter[i] == PORT_COUNT-1 ? 0 : RR_counter[i]+1);
        else
            output_wire->RR_counter_next[i] = RR_counter[i];
    }
    
    // Allocate grant based on RR priority
    for(int i = 0 ; i < PORT_COUNT ; i++){
        #pragma HLS UNROLL
        for(int j = 0 ; j < PORT_COUNT ; j++){
            #pragma HLS UNROLL
            if(RR_counter[j] == i && output_VC_full[j] == false)
                grant[i][j] = request[i][j];
            else
                grant[i][j] = false;
        }
    }
}

void arbiter::update_sequential(
    arbiter_internal_wire input_wire
){
    
    #pragma HLS INLINE
    for(int i = 0 ; i < PORT_COUNT ; i++){
        #pragma HLS UNROLL
        RR_counter[i] = input_wire.RR_counter_next[i];
        flit_counter[i] = input_wire.flit_counter_next[i];
    }
}