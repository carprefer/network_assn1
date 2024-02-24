#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

//template <typename... Targs>
//void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity) {
	size = capacity;
	filled = 0;	
	inputEnd = 0;
}

size_t ByteStream::write(const string &data) {
    size_t dataSize = data.size();
	size_t writtenSize;

	if(dataSize + filled <= capacity) {
		buffer = buffer + data;
		writtenSize = dataSize;
		filled += dataSize;
	}
	else {
		writtenSize = capacity - filled;
		buffer = buffer + data.substr(0, writtenSize);
		filled = capacity;
	}
    return writtenSize;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    return buffer.substr(0, len);
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) { 
	buffer.erase(0, len);
	filled -= len;
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    string copy = peek_output(len);
	pop_output(len);
    return copy;
}

void ByteStream::end_input() {
	inputEnd = 1;
}

bool ByteStream::input_ended() const {
	return inputEnd; 
}

size_t ByteStream::buffer_size() const {
	return ; 
}

bool ByteStream::buffer_empty() const { return {}; }

bool ByteStream::eof() const { return false; }

size_t ByteStream::bytes_written() const { return {}; }

size_t ByteStream::bytes_read() const { return {}; }

size_t ByteStream::remaining_capacity() const { return {}; }
