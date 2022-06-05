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
        
    }
    else {  // Kepp old VC channel
        write_channel = last_ID;
    }   
    
    for(int i = 0 ; i < PORT_COUNT ; i++){
        #pragma HLS UNROLL
        out_flit_data[i] = VC_buffer[i].front();
        out_flit_valid[i] = (!VC_buffer[i].empty()) & read[i];
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

