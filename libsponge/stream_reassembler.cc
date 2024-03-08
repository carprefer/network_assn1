#include "stream_reassembler.hh"
#include <iostream>
// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

using namespace std;

size_t StreamReassembler::update_index(uint64_t index, int size) {
	uint64_t preIdx = 0;
	uint64_t currentIdx = 0;
	int preSize = 0;
	int currentSize = 0;
	bool flag = 0;								// 0: index / 1: size 
	int i = 0;
	int currentLoc = 0;
	int preLoc = 0;
	int myLoc = 0;
	stringLoc = 0;
	rightGap = 0;
	leftGap = 0;
	middleGap = 0;
	char c;
	while(1) {
		c = _index[i];
		if(c == ':') flag = 1;
		else if(c == ' ') {
			uint64_t tempIdx = currentIdx;
			size_t tempSize = currentSize;
			if(index > currentIdx) {	
				preLoc = currentLoc;
				preIdx = currentIdx;
				preSize = currentSize;
				stringLoc += currentSize;
				myLoc = i + 1;
				currentLoc = i + 1;
				flag = 0;
				currentIdx = 0;
				currentSize = 0;
			}
			if(index + size >= tempIdx + tempSize) {
				if(flag) middleGap += tempSize;
				currentLoc = i + 1;
				flag = 0;
				currentIdx = 0;
				currentSize = 0;
			}
			else break;
		}
		else if(c == '|') break;
		else {
			if(flag) {
				currentSize *= 10;
				currentSize += c - '0';
			}
			else {
				currentIdx *= 10;
				currentIdx += c - '0';
			}
		}
		i++;
	}
	uint64_t newIdx;
	int newSize, newLoc;
	if((preIdx && flag) && (index + size >= currentIdx && preIdx + preSize >= index)) {
		_index.erase(preLoc, i - preLoc + 1);
		newLoc = preLoc;
		newIdx = preIdx;
		newSize = max(max(preIdx + preSize, index + size), currentIdx + currentSize) - newIdx;
		rightGap = size - (currentIdx - index);
		leftGap = preSize - (index - preIdx);
	}
	else if( preIdx && (preIdx + preSize >= index)) {
		_index.erase(preLoc, currentLoc-preLoc);
		newLoc = preLoc;
		newIdx = preIdx;
		newSize = max(preIdx + preSize, index + size) - newIdx;
		leftGap = preSize - (index - preIdx);
	}
	else if(flag && (index + size >= currentIdx)) {
		_index.erase(myLoc, i - myLoc + 1);
		newLoc = myLoc;
		newIdx = index;
		newSize = max(index + size, currentIdx + currentSize) - newIdx;
		rightGap = size - (currentIdx - index);
	}
	else {

		_index.erase(myLoc, currentLoc - myLoc);
		newLoc = myLoc;
		newIdx = index;
		newSize = size;
	}
	_index.insert(newLoc, std::to_string(newIdx) + ":" + std::to_string(newSize) + " ");

	if(totalRead + assembled == index) {
		_index.erase(0, _index.find(' ') + 1);
		return newSize;
	}
	return 0;
}


StreamReassembler::StreamReassembler(const size_t capacity) 
: _output(capacity), _capacity(capacity), totalRead(0), assembled(0), eofFlag(0), _index("|"), unassembled(""), stringLoc(0), rightGap(0), leftGap(0), middleGap(0)
{}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {	
	/* update stream in case of reading the bytestream */
	totalRead = _output.bytes_read();
	assembled = _output.buffer_size();

	uint64_t startIdx = totalRead + assembled;				// first index of unassembled scope
	uint64_t endIdx = totalRead + _capacity;				// last index of unassembled scope + 1

	string realData;										// data that is in boundary of unassembled scope
	uint64_t realStart = index;								// realData's first index
	uint64_t realEnd = index + data.size();					// realData's last index + 1

	/* cutting process (to fit in unassembled scope) */
	if(index >= endIdx) return;
	if(index < startIdx) realStart = startIdx;
	if(realEnd > endIdx) realEnd = endIdx;
	else if(eof) eofFlag = 1;								// if receives eof signal and its end index is located with in endIdx, then set eofFlag as 1

	if(realEnd > startIdx) {								// if substrings are already assembled or read, then do nothing. else do below
		size_t realSize = realEnd - realStart;				// realData's size		
		realData = data.substr(realStart - index, realSize);	// make cutted data(realData) from original data
		/* update _index string && other members */
		size_t made = update_index(realStart, realSize);		// update _index information and get size of newly assembled strings
		/* update unassembled string */
		if(realSize > leftGap + rightGap) {						// if realSize is over Gap(it makes it possible to overwrite)
			unassembled.erase(stringLoc, middleGap);			// erase duplicated strings
			unassembled.insert(stringLoc, realData.substr(leftGap, realSize - leftGap - rightGap));	// insert realData string(with Gap cutting)
		}
		/* update _output bytestream */
		if(made) {												// if there exist newly assembled strings				
			_output.write(unassembled.substr(0, made));			// write them in _output bytestream
			unassembled.erase(0, made);							// and erase them in unassembled string
		}
	}

	/* eof setter */
	if(eofFlag && unassembled.size() == 0) _output.end_input();

	return;
}

size_t StreamReassembler::unassembled_bytes() const { return unassembled.size(); }

bool StreamReassembler::empty() const { return _output.buffer_empty() && unassembled.empty(); }
