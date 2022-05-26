#include "util.h"
#include "../src/define.h"



std::vector<flit_t> transform_packet_to_flit(packet_t packet){
    std::vector<flit_t> flits(FLITS_PER_PACKET);
    for(int i = FLITS_PER_PACKET - 1 ; i > 0 ; i--){
        int index = FLITS_PER_PACKET - 1 - i;
        flits[i] = packet.payload.range((index+1)*FLIT_DATA_WIDTH-1, index*FLIT_DATA_WIDTH);
    }
    int index = FLITS_PER_PACKET - 1;
    flits[0] = (packet.seq_num, packet.dest_ID, packet.payload.range(PACKET_PAYLOAD_WIDTH - 1 ,index*FLIT_DATA_WIDTH));

    return flits;
}

packet_t transform_flit_to_packet(std::vector<flit_t> flits){
    /*
    Packet : {seq_num, dest_ID, payload}
    Cut to FLITS_PER_PACKET flits

    flits[0] contains 
    (PACKET_SEQ_NUM_WIDTH)-bit seq_num, 
    (ROUTER_ID_WIDTH)-bit dest_ID, 
    (FLIT_DATA_WIDTH-ROUTER_ID_WIDTH-PACKET_SEQ_NUM_WIDTH)-bit payload data
    
    flits[FLITS_PER_PACKET-1:0] each contains (FLIT_DATA_WIDTH)-bit payload data 
    */
    packet_t packet;
    for(int i = FLITS_PER_PACKET - 1 ; i > 0 ; i--){
        int index = FLITS_PER_PACKET - 1 - i;
        packet.payload.range((index+1)*FLIT_DATA_WIDTH-1, index*FLIT_DATA_WIDTH) = flits[i];
    }
    int index = FLITS_PER_PACKET - 1;
    packet.seq_num = flits[0].range(FLIT_DATA_WIDTH-1, FLIT_DATA_WIDTH-PACKET_SEQ_NUM_WIDTH);
    packet.dest_ID = flits[0].range(FLIT_DATA_WIDTH-PACKET_SEQ_NUM_WIDTH-1, FLIT_DATA_WIDTH-PACKET_SEQ_NUM_WIDTH-ROUTER_ID_WIDTH);
    packet.payload.range(PACKET_PAYLOAD_WIDTH - 1 ,index*FLIT_DATA_WIDTH) = flits[0].range(FLIT_DATA_WIDTH-ROUTER_ID_WIDTH-PACKET_SEQ_NUM_WIDTH-1,0);
    return packet;
}