#include "router_top.h"
#include "define.h"


void router_top::set_ID(ID_t ID){
    #pragma HLS INTERFACE ap_ctrl_none port=return 
    ID = ID;
    for(int i = 0 ; i < PORT_COUNT ; i++){
        #pragma HLS UNROLL
        input_unit_inst[i].router_ID = ID;
        output_unit_inst[i].router_ID = ID;
    }
}
void router_top::update_combinational(
    bool   in_flit_valid[PORT_COUNT],   // input
    bool   out_VC_full[PORT_COUNT],  // input
    flit_t in_flit_data[PORT_COUNT],    // input
    bool   in_VC_full[PORT_COUNT],   // output
    bool   out_flit_valid[PORT_COUNT],   // output
    flit_t out_flit_data[PORT_COUNT],    // output

    router_internal_wire* output_wire   // Output
){
    #pragma HLS INTERFACE ap_ctrl_none port=return 
    
    #pragma HLS INTERFACE ap_none port=in_flit_data[0]
    #pragma HLS INTERFACE ap_none port=in_flit_data[1]
    #pragma HLS INTERFACE ap_none port=in_flit_data[2]
    #pragma HLS INTERFACE ap_none port=in_flit_data[3]
    #pragma HLS INTERFACE ap_none port=in_flit_data[4]
    #pragma HLS INTERFACE ap_none port=in_flit_valid[0]
    #pragma HLS INTERFACE ap_none port=in_flit_valid[1]
    #pragma HLS INTERFACE ap_none port=in_flit_valid[2]
    #pragma HLS INTERFACE ap_none port=in_flit_valid[3]
    #pragma HLS INTERFACE ap_none port=in_flit_valid[4]
    #pragma HLS INTERFACE ap_none port=out_VC_full[0]
    #pragma HLS INTERFACE ap_none port=out_VC_full[1]
    #pragma HLS INTERFACE ap_none port=out_VC_full[2]
    #pragma HLS INTERFACE ap_none port=out_VC_full[3]
    #pragma HLS INTERFACE ap_none port=out_VC_full[4]
    #pragma HLS INTERFACE ap_none port=output_wire

	#pragma HLS ARRAY_PARTITION variable=in_flit_data complete dim=1
	#pragma HLS ARRAY_PARTITION variable=in_flit_valid complete dim=1
	#pragma HLS ARRAY_PARTITION variable=out_VC_full complete dim=1

	#pragma HLS INTERFACE ap_none port=in_VC_full[0]
	#pragma HLS INTERFACE ap_none port=in_VC_full[1]
	#pragma HLS INTERFACE ap_none port=in_VC_full[2]
	#pragma HLS INTERFACE ap_none port=in_VC_full[3]
	#pragma HLS INTERFACE ap_none port=in_VC_full[4]
	#pragma HLS INTERFACE ap_none port=out_flit_valid[0]
	#pragma HLS INTERFACE ap_none port=out_flit_valid[1]
	#pragma HLS INTERFACE ap_none port=out_flit_valid[2]
	#pragma HLS INTERFACE ap_none port=out_flit_valid[3]
	#pragma HLS INTERFACE ap_none port=out_flit_valid[4]
	#pragma HLS INTERFACE ap_none port=out_flit_data[0]
	#pragma HLS INTERFACE ap_none port=out_flit_data[1]
	#pragma HLS INTERFACE ap_none port=out_flit_data[2]
	#pragma HLS INTERFACE ap_none port=out_flit_data[3]
	#pragma HLS INTERFACE ap_none port=out_flit_data[4]

	#pragma HLS ARRAY_PARTITION variable=out_flit_data complete dim=1
	#pragma HLS ARRAY_PARTITION variable=out_flit_valid complete dim=1
	#pragma HLS ARRAY_PARTITION variable=in_VC_full complete dim=1


    #pragma HLS INLINE

    //================================
    // Arbier
    // Must update first, since its combinational signal should pass to other modules
    bool request[PORT_COUNT][PORT_COUNT];
    bool grant[PORT_COUNT][PORT_COUNT];
    bool output_VC_full[PORT_COUNT];
    
	#pragma HLS ARRAY_PARTITION variable=request complete dim=0
	#pragma HLS ARRAY_PARTITION variable=grant complete dim=0
	#pragma HLS ARRAY_PARTITION variable=output_VC_full complete dim=0

    for(int i = 0 ; i < PORT_COUNT ; i++){
        #pragma HLS UNROLL
        for(int j = 0 ; j < PORT_COUNT ; j++){
            #pragma HLS UNROLL
            request[i][j] = !input_unit_inst[i].VC_buffer[j].empty();
        }
    }   
    for(int i = 0 ; i < PORT_COUNT ; i++){
        #pragma HLS UNROLL
        output_VC_full[i] = output_unit_inst[i].VC_buffer.full();
    }

    arbiter_inst.update_combinational(
        output_VC_full,
        request,
        grant,
        &(output_wire->arbiter_wire)
    );
    //================================


    //================================
    // Input units
    flit_t input_unit_inst_out_flit_data[PORT_COUNT][PORT_COUNT];
    bool input_unit_inst_out_flit_valid[PORT_COUNT][PORT_COUNT];
	#pragma HLS ARRAY_PARTITION variable=input_unit_inst_out_flit_data complete dim=0
	#pragma HLS ARRAY_PARTITION variable=input_unit_inst_out_flit_valid complete dim=0
    for(int i = 0 ; i < PORT_COUNT ; i++){
        #pragma HLS UNROLL
        input_unit_inst[i].update_combinational(
            in_flit_data[i],
            in_flit_valid[i],
            grant[i],
            input_unit_inst_out_flit_data[i],
            input_unit_inst_out_flit_valid[i],
            &(in_VC_full[i]),
            &(output_wire->input_unit_wire[i])
        );
    }

    //================================

    

    //================================
    // Output units

    flit_t output_unit_in_flit_data[PORT_COUNT] = {0};
    bool output_unit_in_flit_valid[PORT_COUNT] = {0};
	#pragma HLS ARRAY_PARTITION variable=output_unit_in_flit_data complete dim=0
	#pragma HLS ARRAY_PARTITION variable=output_unit_in_flit_valid complete dim=0

    for(int i = 0 ; i < PORT_COUNT ; i++){
        #pragma HLS UNROLL
        for(int j = 0 ; j < PORT_COUNT ; j++){
            #pragma HLS UNROLL
            if(grant[i][j]){
                output_unit_in_flit_data[j] = input_unit_inst_out_flit_data[i][j];
                output_unit_in_flit_valid[j] = input_unit_inst_out_flit_valid[i][j];
            }
        }
    }

    for(int i = 0 ; i < PORT_COUNT ; i++){
        #pragma HLS UNROLL
        
        output_unit_inst[i].update_combinational(
            output_unit_in_flit_data[i],
            output_unit_in_flit_valid[i],
            out_VC_full[i],
            &(out_flit_data[i]),
            &(out_flit_valid[i]),
            &(output_wire->output_unit_wire[i])
        );
    }
    //================================


    return;
}
void router_top::update_sequential(
    router_internal_wire input_wire      // Input
){
    #pragma HLS INTERFACE ap_ctrl_none port=return 
    #pragma HLS INTERFACE ap_none port=input_wire
    #pragma HLS INLINE
    
    for(int i = 0 ; i < PORT_COUNT ; i++){
        #pragma HLS UNROLL
        input_unit_inst[i].update_sequential(
            input_wire.input_unit_wire[i]
        );
        output_unit_inst[i].update_sequential(
            input_wire.output_unit_wire[i]
        );
    }

    arbiter_inst.update_sequential(
        input_wire.arbiter_wire
    );
    

    return;
}
