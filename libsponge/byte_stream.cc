#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

//template <typename... Targs>
//void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity)					// constructor(don't know why these members should be initialized by initializer list.(because of errors. find later) 
: _error(false), buffer(""), size(capacity), filled(0), inputEnd(0), writtenTotal(0), readTotal(0)
{}

size_t ByteStream::write(const string &data) {
    size_t dataSize = data.size();						// input data length
	size_t realSize = min(dataSize, size - filled);		// real length(because of limit of capacity)

	buffer += data.substr(0, realSize);					// update buffer(adds data behind buffer)

	filled += realSize;									// update current buffer's status
	writtenTotal += realSize;							// update total written size
    return realSize;									// return the size written in buffer by this function
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    return buffer.substr(0, min(len, filled));			// return contents of buffer(0~ up to filled)
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) { 
	size_t realLen = min(len, filled);					// real length that can be poped out(because of limit of capacity)
	buffer.erase(0, realLen);							// remove front parts of buffer
	filled -= realLen;									// update current buffer's size
	readTotal += len;									// updata the size read by this function
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    string copy = peek_output(len);						// store contents that will be popped out(for return) 
	pop_output(len);									// remove len length of strings in buffer
    return copy;										// return read strings
}

void ByteStream::end_input() {
	inputEnd = true;									// make a signal that inputs have ended.
}

bool ByteStream::input_ended() const {
	return inputEnd; 									// return whether the inputs ended or not
}

size_t ByteStream::buffer_size() const {
	return filled; 										// return the filled size of the buffer.
}

bool ByteStream::buffer_empty() const {
	return !filled;										// return whether the buffer is empty or not
}

bool ByteStream::eof() const {
	return input_ended() && buffer_empty(); 			// if inputs are ended and buffer is empty, than make eof() signal
}

size_t ByteStream::bytes_written() const {
	return writtenTotal;								// return total written bytes.
}

size_t ByteStream::bytes_read() const {
	return readTotal; 									// return total read bytes.
}

size_t ByteStream::remaining_capacity() const {
	return size - filled;								// return remaining bytes in buffer
}
