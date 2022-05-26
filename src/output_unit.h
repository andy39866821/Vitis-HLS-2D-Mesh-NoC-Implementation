#ifndef __OUTPUT_UNIT_H__
#define __OUTPUT_UNIT_H__
#include "flit_buffer.h"
#include "define.h"

typedef struct{
    flit_buffer_internal_wire flit_buffer_wire;
} output_unit_internal_wire;

class output_unit
{
public:
    ID_t router_ID;
    flit_buffer VC_buffer;
    output_unit();

    void update_combinational(
        flit_t  in_flit_data,       // input
        bool    in_flit_valid,      // input
        bool    out_VC_full,        // input
        flit_t* out_flit_data,      // output
        bool*   out_flit_valid,     // output
        output_unit_internal_wire* output_wire   // output
    );

    void update_sequential(
        output_unit_internal_wire input_wire
    );
};






#endif