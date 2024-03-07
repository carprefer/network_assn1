#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) 
: _output(capacity), _capacity(capacity), totalRead(0), assembled(0), unassembled(""), _index("|"), stringLoc(0), rightGap(0), leftGap(0), eofFlag(0)
{}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {											// substring's size(<=1640?)
	/* update stream in case of reading the bytestream */
	totalRead = _output.bytes_read();
	assembled = _output.buffer_size();

	uint64_t startIdx = totalRead + assembled;
	uint64_t endIdx = totalRead + _capacity;
	string realData = data;
	size_t realIdx = index;
	size_t realSize = realData.size();

	if(index >= endIdx) return;
	else if(index < startIdx) {
		if(index + data.size() < startIdx) return;
		else {
			realData = realData.substr(startIdx - index);
			realIdx = startIdx;
			realSize -= (startIdx - index);
		}
	}
	if(realIdx + realSize > endIdx) {
		realData.erase(endIdx - realIdx, realSize - endIdx + realIdx);
		realSize -= (realSize - endIdx + realIdx);
	}
	else if(eof) eofFlag = 1;

	int made = update_index(realIdx, realData.size());
	if(realData.size() > leftGap + rightGap) 
		unassembled.insert(stringLoc + leftGap, realData.substr(leftGap, realData.size() - leftGap - rightGap));
	if(made) {
		assembled += made;
		_output.write(unassembled.substr(0, made));
		unassembled.erase(0, made);
	}

	if(eofFlag && unassembled.size() == 0) _output.end_input();
	return;
}

size_t StreamReassembler::unassembled_bytes() const { return unassembled.size(); }

bool StreamReassembler::empty() const { return _output.buffer_empty() && unassembled.empty(); }
