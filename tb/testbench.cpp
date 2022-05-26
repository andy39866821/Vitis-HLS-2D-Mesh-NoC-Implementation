#include "../src/system_top.h"
#include "util.h"
#include <time.h>
#include <vector>
#include <hls_stream.h>
#include <fstream>
using namespace std;

int main() {

    srand(time(NULL));

    const int pattern_num = 2000;
    int cycle_count = 0;
    // Generate patterns
    fstream file;
    vector<packet_t> input_packets_array;
    flit_t input_flits_array[ROUTER_COUNT][SYSTEM_TOP_MEM_SIZE];
    flit_t output_flits_array[ROUTER_COUNT][SYSTEM_TOP_MEM_SIZE];
    
    int output_flit_count[ROUTER_COUNT];
    int port_index = 0 , flit_index = 0;
    file.open("pattern.dat", ios::in);
    for(int i = 0 ; i < pattern_num ; i++){
        packet_t packet;

        file >> packet.dest_ID;
        file >> packet.seq_num;
        file >> packet.payload;
        input_packets_array.push_back(packet);

        auto flits = transform_packet_to_flit(packet); 
        for(int j = 0 ; j < FLITS_PER_PACKET ; j++){
            input_flits_array[port_index][flit_index] = flits[j];
            flit_index++;
        }
        if(flit_index == pattern_num/ROUTER_COUNT*FLITS_PER_PACKET){
            port_index++;
            flit_index = 0;
        }
    }

    // Compute Golden
    vector<packet_t> golden_output_packets[ROUTER_COUNT];
    for(int i = 0 ; i < pattern_num ; i++){
        golden_output_packets[input_packets_array[i].dest_ID%ROUTER_COUNT].push_back(input_packets_array[i]);
    }

    for(int i = 0 ; i < ROUTER_COUNT ; i++){
        printf("Golden flits[%d] count:%d\n", i, golden_output_packets[i].size()*FLITS_PER_PACKET);
    }
    
    system_top(
        input_flits_array,
        output_flits_array,
        output_flit_count,
        pattern_num,
        &cycle_count
    );

    for(int i = 0 ; i < ROUTER_COUNT ; i++){
        printf("Out flit[%d] count %d\n", i , output_flit_count[i]);
    }
    printf("Total Cycle: %d\n", cycle_count);

    return 0;
}