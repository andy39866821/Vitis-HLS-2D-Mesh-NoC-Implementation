#ifndef __UTIL_H__
#define __UTIL_H__

#include <vector>
#include "../src/define.h"

std::vector<flit_t> transform_packet_to_flit(packet_t);
packet_t transform_flit_to_packet(std::vector<flit_t>);


#endif