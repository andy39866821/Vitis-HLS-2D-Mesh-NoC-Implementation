#ifndef __DEFINE_H__
#define __DEFINE_H__

#include <ap_int.h>
#include <hls_stream.h>

#define SYSTEM_TOP_MEM_SIZE 8192
#define ROUTER_ID_WIDTH 4
#define PACKET_PAYLOAD_WIDTH 512    // Payload width
#define PACKET_SEQ_NUM_WIDTH 4      // Sequence number width
#define FLITS_PER_PACKET 5
#define FLIT_DATA_WIDTH 104
#define VC_BUFFER_SIZE 64

#define ROUTER_ROW_COUNT 2
#define ROUTER_COL_COUNT 2
#define ROUTER_COUNT (ROUTER_ROW_COUNT*ROUTER_COL_COUNT)


#define PORT_COUNT 5 // Only NORTH and SOUTH
#define VC_NORTH 0
#define VC_SOUTH 1
#define VC_EAST  2
#define VC_WEST  3
#define VC_NI    4


typedef ap_uint<ROUTER_ID_WIDTH> ID_t;
typedef ap_uint<PACKET_SEQ_NUM_WIDTH> seq_num_t;
typedef ap_uint<FLIT_DATA_WIDTH> flit_t;

typedef struct{
    ap_uint<PACKET_PAYLOAD_WIDTH> payload;
    seq_num_t seq_num;
    ID_t dest_ID;
} packet_t;
 
#endif