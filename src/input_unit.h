#ifndef __INPUT_UNIT_H__
#define __INPUT_UNIT_H__
#include "flit_buffer.h"
#include "define.h"

typedef struct{
    int flit_counter_next;
    ID_t last_ID_next;
    flit_t write_data;
    flit_buffer_internal_wire flit_buffer_wire[PORT_COUNT];
} input_unit_internal_wire;

class input_unit
{
private:
    //flit_buffer<VC_BUFFER_SIZE> VC_buffer[PORT_COUNT];
    int flit_counter = 0;
    ID_t last_ID = 0;
public:
    flit_buffer VC_buffer[PORT_COUNT];
    ID_t router_ID = 0;
    
    input_unit();
    

    void update_combinational(
        flit_t  in_flit_data,       // input
        bool    in_flit_valid,      // input
        bool    read[PORT_COUNT],   // input
        flit_t  out_flit_data[PORT_COUNT],      // output
        bool    out_flit_valid[PORT_COUNT],     // output
        bool*   VC_full,                        // output 
        input_unit_internal_wire* output_wire   // output
    );

    void update_sequential(
        input_unit_internal_wire input_wire
    );
};


#endif