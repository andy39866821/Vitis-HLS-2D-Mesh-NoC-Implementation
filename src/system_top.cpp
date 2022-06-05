
#include "system_top.h"
#include "router_top.h"

void system_top(
    flit_t input_flit_array[ROUTER_COUNT][SYSTEM_TOP_MEM_SIZE],
    flit_t output_flit_array[ROUTER_COUNT][SYSTEM_TOP_MEM_SIZE],
    int output_flit_count[ROUTER_COUNT],
    int pattern_num,
    int *cycle_count
){
    #pragma HLS INTERFACE ap_ctrl_hs port=return 
	#pragma HLS ARRAY_PARTITION variable=input_flit_array complete dim=1
	#pragma HLS ARRAY_PARTITION variable=output_flit_array complete dim=1
	#pragma HLS ARRAY_PARTITION variable=output_flit_count complete dim=1
    

    router_top router_inst[ROUTER_COUNT];
    for(int i = 0 ; i < ROUTER_COUNT ; i++){
        #pragma HLS UNROLL
        router_inst[i].set_ID((ID_t)i);
    }
    
    int input_index[ROUTER_COUNT] = {0};
    int output_index[ROUTER_COUNT] = {0};
    int total_output_count = 0;
    int cycle = 0;
    
    bool   in_VC_full[ROUTER_COUNT][PORT_COUNT] = {0};  // output
    flit_t out_flit_data[ROUTER_COUNT][PORT_COUNT] = {0};    // output
    bool   out_flit_valid[ROUTER_COUNT][PORT_COUNT] = {0};   // output

    system_loop: 
    while(total_output_count < pattern_num*FLITS_PER_PACKET){
        #pragma HLS PIPELINE II=1

        router_internal_wire wire_inst[ROUTER_COUNT];
        
        flit_t in_flit_data[ROUTER_COUNT][PORT_COUNT];    // input
        bool   in_flit_valid[ROUTER_COUNT][PORT_COUNT];   // input
        bool   out_VC_full[ROUTER_COUNT][PORT_COUNT];  // input

        for(int i = 0 ; i < ROUTER_COUNT ; i++){
            #pragma HLS UNROLL
            if(input_index[i] < pattern_num*FLITS_PER_PACKET/ROUTER_COUNT && in_VC_full[i][VC_NI] == false){
                in_flit_data[i][VC_NI] = input_flit_array[i][input_index[i]];
                in_flit_valid[i][VC_NI] = true;
                input_index[i]++;
            }
            else {
                in_flit_data[i][VC_NI] = 0;
                in_flit_valid[i][VC_NI] = false;
            }
        }

        
        for(int i = 0 ; i < ROUTER_COUNT ; i++){
            #pragma HLS UNROLL
            if(out_flit_valid[i][VC_NI] == true){
                output_flit_array[i][output_index[i]] = out_flit_data[i][VC_NI]; 
                out_VC_full[i][VC_NI] = false;
                output_index[i]++;
                total_output_count++;
            }
            else {
                out_VC_full[i][VC_NI] = false;
            }
        }
        

        for(int i = 0 ; i < ROUTER_COUNT ; i++){
            #pragma HLS UNROLL
            if(i-ROUTER_ROW_COUNT >= 0){
                out_VC_full[i][VC_NORTH] = in_VC_full[i-ROUTER_ROW_COUNT][VC_SOUTH];
                in_flit_data[i][VC_NORTH] = out_flit_data[i-ROUTER_ROW_COUNT][VC_SOUTH];
                in_flit_valid[i][VC_NORTH] = out_flit_valid[i-ROUTER_ROW_COUNT][VC_SOUTH];
            }
            else {
                out_VC_full[i][VC_NORTH] = in_VC_full[ROUTER_COUNT-ROUTER_COL_COUNT+i][VC_SOUTH];
                in_flit_data[i][VC_NORTH] = out_flit_data[ROUTER_COUNT-ROUTER_COL_COUNT+i][VC_SOUTH];
                in_flit_valid[i][VC_NORTH] = out_flit_valid[ROUTER_COUNT-ROUTER_COL_COUNT+i][VC_SOUTH];
            }
            
            
            if(i+ROUTER_ROW_COUNT < ROUTER_COUNT){
                out_VC_full[i][VC_SOUTH] = in_VC_full[i+ROUTER_ROW_COUNT][VC_NORTH];
                in_flit_data[i][VC_SOUTH] = out_flit_data[i+ROUTER_ROW_COUNT][VC_NORTH];
                in_flit_valid[i][VC_SOUTH] = out_flit_valid[i+ROUTER_ROW_COUNT][VC_NORTH];
            }
            else {
                out_VC_full[i][VC_SOUTH] = in_VC_full[i%ROUTER_COL_COUNT][VC_NORTH];
                in_flit_data[i][VC_SOUTH] = out_flit_data[i%ROUTER_COL_COUNT][VC_NORTH];
                in_flit_valid[i][VC_SOUTH] = out_flit_valid[i%ROUTER_COL_COUNT][VC_NORTH];
            }
            
            if((i%ROUTER_ROW_COUNT) > 0){
                out_VC_full[i][VC_WEST] = in_VC_full[i-1][VC_EAST];
                in_flit_data[i][VC_WEST] = out_flit_data[i-1][VC_EAST];
                in_flit_valid[i][VC_WEST] = out_flit_valid[i-1][VC_EAST];
            }
            else {
                out_VC_full[i][VC_WEST] = in_VC_full[i+ROUTER_COL_COUNT-1][VC_EAST];
                in_flit_data[i][VC_WEST] = out_flit_data[i+ROUTER_COL_COUNT-1][VC_EAST];
                in_flit_valid[i][VC_WEST] = out_flit_valid[i+ROUTER_COL_COUNT-1][VC_EAST];
            }

            if((i%ROUTER_ROW_COUNT) < ROUTER_COL_COUNT-1){
                out_VC_full[i][VC_EAST] = in_VC_full[i+1][VC_WEST];
                in_flit_data[i][VC_EAST] = out_flit_data[i+1][VC_WEST];
                in_flit_valid[i][VC_EAST] = out_flit_valid[i+1][VC_WEST];
            }
            else {
                out_VC_full[i][VC_EAST] = in_VC_full[i+1-ROUTER_COL_COUNT][VC_WEST];
                in_flit_data[i][VC_EAST] = out_flit_data[i+1-ROUTER_COL_COUNT][VC_WEST];
                in_flit_valid[i][VC_EAST] = out_flit_valid[i+1-ROUTER_COL_COUNT][VC_WEST];
            }
        }

        router_update_block:{
            #pragma HLS LATENCY max=0
            for(int i = 0 ; i < ROUTER_COUNT ; i++){
                #pragma HLS UNROLL
                router_inst[i].update_combinational( 
                    in_flit_valid[i],   // input
                    out_VC_full[i],  // input
                    in_flit_data[i],    // input
                    in_VC_full[i],   // output
                    out_flit_valid[i],   // output
                    out_flit_data[i],    // output
                    &wire_inst[i]
                );
            }


            for(int i = 0 ; i < ROUTER_COUNT ; i++){
                #pragma HLS UNROLL
                router_inst[i].update_sequential(
                    wire_inst[i]
                );
            }
        }
        cycle++;
    }

    *cycle_count = cycle;

    output_flit_count_loop: 
    for(int i = 0 ; i < ROUTER_COUNT ; i++){
        #pragma HLS UNROLL
        output_flit_count[i] = output_index[i];
    }
    return;
}

