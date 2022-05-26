#include "input_unit.h"

input_unit::input_unit() {

    //#pragma HLS INTERFACE ap_ctrl_none port=return
}

void input_unit::update_combinational(
    flit_t  in_flit_data,       // input
    bool    in_flit_valid,      // input
    bool    read[PORT_COUNT],   // input
    flit_t  out_flit_data[PORT_COUNT],      // output
    bool    out_flit_valid[PORT_COUNT],     // output
    bool    *VC_full,                       // output 
    input_unit_internal_wire* output_wire   // output
) {
    
    //#pragma HLS INTERFACE ap_ctrl_none port=return
    // #pragma HLS INTERFACE ap_none port=in_flit_data
    // #pragma HLS INTERFACE ap_none port=in_flit_valid
    // #pragma HLS INTERFACE ap_none port=read[0]
    // #pragma HLS INTERFACE ap_none port=read[1]
    // #pragma HLS INTERFACE ap_none port=read[2]
    // #pragma HLS INTERFACE ap_none port=read[3]
    // #pragma HLS INTERFACE ap_none port=out_flit_data[0]
    // #pragma HLS INTERFACE ap_none port=out_flit_data[1]
    // #pragma HLS INTERFACE ap_none port=out_flit_data[2]
    // #pragma HLS INTERFACE ap_none port=out_flit_data[3]
    // #pragma HLS INTERFACE ap_none port=out_flit_valid[0]
    // #pragma HLS INTERFACE ap_none port=out_flit_valid[1]
    // #pragma HLS INTERFACE ap_none port=out_flit_valid[2]
    // #pragma HLS INTERFACE ap_none port=out_flit_valid[3]
    // #pragma HLS INTERFACE ap_none port=VC_full
    // #pragma HLS INTERFACE ap_none port=output_wire
    
	// #pragma HLS ARRAY_PARTITION variable=read complete dim=0
	// #pragma HLS ARRAY_PARTITION variable=out_flit_data complete dim=0
	// #pragma HLS ARRAY_PARTITION variable=out_flit_valid complete dim=0

    #pragma HLS INLINE
    int write_channel;

    if(flit_counter == 0 && in_flit_valid == true){  // Decide new VC chaneel
        ID_t flit_ID = in_flit_data.range(FLIT_DATA_WIDTH-PACKET_SEQ_NUM_WIDTH-1, FLIT_DATA_WIDTH-PACKET_SEQ_NUM_WIDTH-ROUTER_ID_WIDTH);

        if(flit_ID == router_ID){
            write_channel = VC_NI;
        }
        else if(flit_ID.get_bit(1) == router_ID.get_bit(1)){ // Same row

            // Need change back
            if(flit_ID.get_bit(0) > router_ID.get_bit(0))
                write_channel = VC_WEST;
            else
                write_channel = VC_EAST;
        }
        else {
            if(flit_ID.get_bit(1) > router_ID.get_bit(1))
                write_channel = VC_NORTH;
            else
                write_channel = VC_SOUTH;
        }
        
        //printf("Router[%d], dest_ID %d, write_channel:%d\n", (int)router_ID, (int)flit_ID, write_channel);
    }
    else {  // Kepp old VC channel
        write_channel = last_ID;
        
       //printf("Router[%d], keep write_channel:%d\n", (int)router_ID, write_channel);
    }   
    //printf("write_channel:%d\n", write_channel);
    for(int i = 0 ; i < PORT_COUNT ; i++){
        #pragma HLS UNROLL
        out_flit_data[i] = VC_buffer[i].front();
        out_flit_valid[i] = (!VC_buffer[i].empty()) & read[i];
        //printf("read:%d read index[%d]: %d\n", (int)read[i], i, VC_buffer[i].read_index);
    }

    if(in_flit_valid == true)
        output_wire->flit_counter_next = (flit_counter == FLITS_PER_PACKET-1 ? 0 : flit_counter+1);
    else
        output_wire->flit_counter_next = flit_counter;

    if(flit_counter == 0 && in_flit_valid == true){  // Decide new VC chaneel
        output_wire->last_ID_next = write_channel;
    }
    else {  // Kepp old VC channel
        output_wire->last_ID_next = last_ID;
    }   

    bool VC_buffer_write[PORT_COUNT];
    for(int i = 0 ; i < PORT_COUNT ; i++){
        #pragma HLS UNROLL
        if(in_flit_valid == true && write_channel == i)
            VC_buffer_write[i] = true;
        else   
            VC_buffer_write[i] = false;
    }

    for(int i = 0 ; i < PORT_COUNT ; i++){
        #pragma HLS UNROLL
        VC_buffer[i].update_combinational(
            in_flit_data,
            read[i],
            VC_buffer_write[i],
            &(output_wire->flit_buffer_wire[i])
        );

        // if(router_ID == 0 && i==VC_EAST){
        //     printf("read:%d, read_idx:%d, next_r_idx:%d write:%d, write_idx:%d, next_w_idx:%d data:%d\n",
        //         read[i],
        //         VC_buffer[i].read_index,
        //         output_wire->flit_buffer_wire[i].read_index_next,
        //         VC_buffer_write[i],
        //         VC_buffer[i].write_index,
        //         output_wire->flit_buffer_wire[i].write_index_next,
        //         (int)in_flit_data.range(FLIT_DATA_WIDTH-PACKET_SEQ_NUM_WIDTH-1, FLIT_DATA_WIDTH-PACKET_SEQ_NUM_WIDTH-ROUTER_ID_WIDTH));
        // }
    }

    int VC_full_temp = false;
    for(int i = 0 ; i < PORT_COUNT ; i++){
        #pragma HLS UNROLL
        VC_full_temp |= VC_buffer[i].full();
    }

    *VC_full = VC_full_temp;
}

void input_unit::update_sequential(
    input_unit_internal_wire input_wire
) {
    
    //#pragma HLS INTERFACE ap_ctrl_none port=return
    //#pragma HLS INTERFACE ap_none port=input_wire
    #pragma HLS INLINE
    
    last_ID = input_wire.last_ID_next;
    flit_counter = input_wire.flit_counter_next;

    for(int i = 0 ; i < PORT_COUNT ; i++){
        #pragma HLS UNROLL
        VC_buffer[i].update_sequential(
            input_wire.flit_buffer_wire[i]
        );
    }

}

