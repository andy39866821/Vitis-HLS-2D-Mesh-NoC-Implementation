#ifndef __ROUTER_TOP_H__
#define __ROUTER_TOP_H__

#include <ap_int.h>
#include <hls_stream.h>
#include "define.h"
#include "flit_buffer.h"
#include "input_unit.h"
#include "output_unit.h"
#include "arbiter.h"

typedef struct{
    // Combinational wires
    input_unit_internal_wire input_unit_wire[PORT_COUNT];
    output_unit_internal_wire output_unit_wire[PORT_COUNT];
    arbiter_internal_wire arbiter_wire;
    
    bool   in_VC_full_next[PORT_COUNT];
    bool   out_flit_valid_next[PORT_COUNT];
    flit_t out_flit_data_next[PORT_COUNT];

} router_internal_wire;

class router_top {
private:

    ID_t ID;
    // Sequential registers / memory
    
public:

    input_unit input_unit_inst[PORT_COUNT];
    output_unit output_unit_inst[PORT_COUNT];
    arbiter arbiter_inst;
    void set_ID(ID_t ID);
    void update_combinational(
        bool   in_flit_valid[PORT_COUNT],   // input
        bool   out_VC_full[PORT_COUNT],  // input
        flit_t in_flit_data[PORT_COUNT],    // input
        bool   in_VC_full[PORT_COUNT],   // output
        bool   out_flit_valid[PORT_COUNT],   // output
        flit_t out_flit_data[PORT_COUNT],    // output
        router_internal_wire* output_wire   // Output
    );
    
    void update_sequential(
        router_internal_wire input_wire      // Input
    );
};

#endif
