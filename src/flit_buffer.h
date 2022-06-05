#ifndef __FLIT_BUFFER_H__
#define __FLIT_BUFFER_H__

#include <ap_int.h>
#include <ap_shift_reg.h>
#include "define.h"
#define FIFO_DEPTH VC_BUFFER_SIZE

typedef struct{
 	int read_index_next; 
	int write_index_next;
	int lengh_next;
	int write_addr;
	bool write_enable;
	flit_t write_data;
}flit_buffer_internal_wire;


//template<int FIFO_DEPTH>
class flit_buffer{
private:
	int read_index  = 0;
	int write_index = 0;
	int lengh = 0;
	flit_t preloaded_data;
    flit_t buffer[FIFO_DEPTH];
public:
	
    flit_buffer(){
		#pragma HLS bind_storage variable=buffer type=RAM_S2P impl=BRAM latency=1
	}
	bool empty(){
    	#pragma HLS INLINE
		return (lengh == 0);
	}

	bool full(){
    	#pragma HLS INLINE
		return (lengh == FIFO_DEPTH);
	}
	
	int size(){
    	#pragma HLS INLINE
		return lengh;
	}

	flit_t front(){
    	#pragma HLS INLINE
		return preloaded_data;
	}

	void update_combinational(
		flit_t data,
		bool read,
		bool write,
		flit_buffer_internal_wire* output_wire
	){
		
    	#pragma HLS INLINE
		if(read == true){
			output_wire->read_index_next = (read_index == FIFO_DEPTH-1 ? 0 : read_index+1);
		}
		else {
			output_wire->read_index_next = read_index;
		}

		if(write == true){
			output_wire->write_index_next = (write_index == FIFO_DEPTH-1 ? 0 : write_index+1);
			output_wire->write_data = data;
			output_wire->write_addr = write_index;
			output_wire->write_enable = true;
		}
		else {
			
			output_wire->write_index_next = write_index;
			output_wire->write_data = 0;
			output_wire->write_addr = 0;
			output_wire->write_enable = false;
		}

		output_wire->lengh_next = lengh - read + write;
		
		if(write && write_index == output_wire->read_index_next)
			preloaded_data = data;
		else
			preloaded_data = buffer[output_wire->read_index_next];
	}

	void update_sequential(
		flit_buffer_internal_wire input_wire
	){

    	#pragma HLS INLINE
	
		read_index = input_wire.read_index_next;
		write_index = input_wire.write_index_next;
		lengh = input_wire.lengh_next;

		if(input_wire.write_enable)
			buffer[input_wire.write_addr] = input_wire.write_data;

	}
};


#endif
