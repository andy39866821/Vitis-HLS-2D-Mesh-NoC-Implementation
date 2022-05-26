#include "output_unit.h"

output_unit::output_unit(/* args */) {

    //#pragma HLS INTERFACE ap_ctrl_none port=return
    //printf("Initial VC_buffer size:%d\n", VC_buffer.size());
}

void output_unit::update_combinational(
    flit_t  in_flit_data,       // input
    bool    in_flit_valid,      // input
    bool    out_VC_full,        // input
    flit_t* out_flit_data,      // output
    bool*   out_flit_valid,     // output
    output_unit_internal_wire* output_wire   // output
) {
    
    //#pragma HLS INTERFACE ap_ctrl_none port=return
    // #pragma HLS INTERFACE ap_none port=in_flit_data
    // #pragma HLS INTERFACE ap_none port=in_flit_valid
    // #pragma HLS INTERFACE ap_none port=out_VC_full
    // #pragma HLS INTERFACE ap_none port=out_flit_data
    // #pragma HLS INTERFACE ap_none port=out_flit_valid
    // #pragma HLS INTERFACE ap_none port=output_wire

    #pragma HLS INLINE
    bool flit_buffer_read;
    if(out_VC_full == false && VC_buffer.empty() == false){
        *out_flit_data = VC_buffer.front();
        //*out_flit_valid = false;
        //flit_buffer_read = false;
        *out_flit_valid = true;
        flit_buffer_read = true;
    }
    else {
        *out_flit_data = 0;
        *out_flit_valid = false;
        flit_buffer_read = false;
    }


    VC_buffer.update_combinational(
        in_flit_data,
        flit_buffer_read,
        in_flit_valid,
        &(output_wire->flit_buffer_wire)
    );

}

void output_unit::update_sequential(
    output_unit_internal_wire input_wire
) {
    
    //#pragma HLS INTERFACE ap_ctrl_none port=return
    //#pragma HLS INTERFACE ap_none port=input_wire
    #pragma HLS INLINE

    VC_buffer.update_sequential(
        input_wire.flit_buffer_wire
    );
    
    // if(router_ID == 0) {
    //     //printf("Write %d %d\n",(int)input_wire.flit_buffer_wire.write_enable, (int)input_wire.flit_buffer_wire.write_data.range(FLIT_DATA_WIDTH-PACKET_SEQ_NUM_WIDTH-1, FLIT_DATA_WIDTH-PACKET_SEQ_NUM_WIDTH-ROUTER_ID_WIDTH));
    //     printf("[%d] Output VC_buffer size:%d\n", (int)router_ID, VC_buffer.size());
    //     // for(int i = 0 ; i < 10 ; i++){
    //     //     printf("Buffer[%d]: %d\n", i , (int)VC_buffer.buffer[(VC_buffer.read_index+i)%64].range(FLIT_DATA_WIDTH-PACKET_SEQ_NUM_WIDTH-1, FLIT_DATA_WIDTH-PACKET_SEQ_NUM_WIDTH-ROUTER_ID_WIDTH));
    //     // }
    // }
       
}

