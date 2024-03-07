#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) 
: _output(capacity), _capacity(capacity), totalRead(0), assembled(0), unassembled(""), _index("|"), stringLoc(0)
{}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    
	int dataSize = data.size();											// substring's size(<=1640?)
	/* update stream in case of reading the bytestream */
	totalRead = _output.bytes_read();
	assembled = _output.buffer_size();

	uint64_t startIdx = totalRead + assembled;
	uint64_t endIdx = totalRead + _capacity;
	
	if(index >= endIdx) return;
	else if(index < startIdx) {
		if(index + dataSize < startIdx) return;
		else {
			//cutting
		}
	}
	int made = update_index(index, dataSize);
	unassembled.insert(stringLoc, data);
	if(made) {
		assembled += made;
		_output.write(unassembled.substr(0, made));
		unassembled.erase(0, made);
	}


	if(eof) _output.end_input();							// if it receives eof signal, tells bytestream that the input is ended.
	return;
}

size_t StreamReassembler::unassembled_bytes() const { return unassembled.size(); }

bool StreamReassembler::empty() const { return _output.buffer_empty() && unassembled.empty(); }
