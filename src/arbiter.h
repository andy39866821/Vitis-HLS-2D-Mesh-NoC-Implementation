#ifndef __ARBITER_H__
#define __ARBITER_H__

#include "define.h"

typedef struct
{
    // Combinational wires
    int RR_counter_next[PORT_COUNT];
    int flit_counter_next[PORT_COUNT];
} arbiter_internal_wire;

class arbiter
{
private:
    int RR_counter[PORT_COUNT] = {0};
    int flit_counter[PORT_COUNT] = {0};

public:
    arbiter(/* args */);

    void update_combinational(
        bool output_VC_full[PORT_COUNT],
        bool request[PORT_COUNT][PORT_COUNT],
        bool grant[PORT_COUNT][PORT_COUNT],
        arbiter_internal_wire *output_wire);

    void update_sequential(
        arbiter_internal_wire input_wire);
};

#endif