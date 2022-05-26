#include "arbiter.h"



arbiter::arbiter() {

}

void arbiter::update_combinational(
    bool output_VC_full[PORT_COUNT],
    bool request[PORT_COUNT][PORT_COUNT],   // Input
    bool grant[PORT_COUNT][PORT_COUNT],     // Output
    arbiter_internal_wire* output_wire
){
    
    //#pragma HLS INTERFACE ap_ctrl_none port=return

    // #pragma HLS INTERFACE ap_none port=output_VC_full[0]
    // #pragma HLS INTERFACE ap_none port=output_VC_full[1]
    // #pragma HLS INTERFACE ap_none port=output_VC_full[2]
    // #pragma HLS INTERFACE ap_none port=output_VC_full[3]

    // #pragma HLS INTERFACE ap_none port=request[0][0]
    // #pragma HLS INTERFACE ap_none port=request[0][1]
    // #pragma HLS INTERFACE ap_none port=request[0][2]
    // #pragma HLS INTERFACE ap_none port=request[0][3]
    // #pragma HLS INTERFACE ap_none port=request[1][0]
    // #pragma HLS INTERFACE ap_none port=request[1][1]
    // #pragma HLS INTERFACE ap_none port=request[1][2]
    // #pragma HLS INTERFACE ap_none port=request[1][3]
    // #pragma HLS INTERFACE ap_none port=request[2][0]
    // #pragma HLS INTERFACE ap_none port=request[2][1]
    // #pragma HLS INTERFACE ap_none port=request[2][2]
    // #pragma HLS INTERFACE ap_none port=request[2][3]
    // #pragma HLS INTERFACE ap_none port=request[3][0]
    // #pragma HLS INTERFACE ap_none port=request[3][1]
    // #pragma HLS INTERFACE ap_none port=request[3][2]
    // #pragma HLS INTERFACE ap_none port=request[3][3]

    // #pragma HLS INTERFACE ap_none port=grant[0][0]
    // #pragma HLS INTERFACE ap_none port=grant[0][1]
    // #pragma HLS INTERFACE ap_none port=grant[0][2]
    // #pragma HLS INTERFACE ap_none port=grant[0][3]
    // #pragma HLS INTERFACE ap_none port=grant[1][0]
    // #pragma HLS INTERFACE ap_none port=grant[1][1]
    // #pragma HLS INTERFACE ap_none port=grant[1][2]
    // #pragma HLS INTERFACE ap_none port=grant[1][3]
    // #pragma HLS INTERFACE ap_none port=grant[2][0]
    // #pragma HLS INTERFACE ap_none port=grant[2][1]
    // #pragma HLS INTERFACE ap_none port=grant[2][2]
    // #pragma HLS INTERFACE ap_none port=grant[2][3]
    // #pragma HLS INTERFACE ap_none port=grant[3][0]
    // #pragma HLS INTERFACE ap_none port=grant[3][1]
    // #pragma HLS INTERFACE ap_none port=grant[3][2]
    // #pragma HLS INTERFACE ap_none port=grant[3][3]
    // #pragma HLS INTERFACE ap_none port=output_wire
    
	// #pragma HLS ARRAY_PARTITION variable=output_VC_full complete dim=1
	// #pragma HLS ARRAY_PARTITION variable=request complete dim=1
	// #pragma HLS ARRAY_PARTITION variable=grant complete dim=1

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
            //printf("Grant[%d][%d]: %d\n",i ,j, grant[i][j],  grant[i][j]);
        }
    }
}

void arbiter::update_sequential(
    arbiter_internal_wire input_wire
){
    //#pragma HLS INTERFACE ap_ctrl_none port=return
    //#pragma HLS INTERFACE ap_none port=input_wire
    
    #pragma HLS INLINE
    for(int i = 0 ; i < PORT_COUNT ; i++){
        #pragma HLS UNROLL
        RR_counter[i] = input_wire.RR_counter_next[i];
        flit_counter[i] = input_wire.flit_counter_next[i];
    }
}