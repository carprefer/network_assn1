#ifndef SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH
#define SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH

#include "byte_stream.hh"

#include <cstdint>
#include <string>

//! \brief A class that assembles a series of excerpts from a byte stream (possibly out of order,
//! possibly overlapping) into an in-order byte stream.
class StreamReassembler {
  private:
    // Your code here -- add private members as necessary.

    ByteStream _output;  //!< The reassembled in-order byte stream
    size_t _capacity;    //!< The maximum number of bytes
	
	uint64_t totalRead;
	size_t assembled;
	std::string unassembled;
	std::string _index;
	int stringLoc;
	size_t rightGap;
	size_t leftGap;
	bool eofFlag;
	
	int update_index(uint64_t index, int size) {
		using std::max;
		uint64_t preIdx = 0;
		uint64_t currentIdx = 0;
		int preSize = 0;
		int currentSize = 0;
		bool flag = 0;								// 0: index / 1: size 
		int i = 0;
		int currentLoc = 0;
		int preLoc = 0;
		stringLoc = 0;
		rightGap = 0;
		leftGap = 0;
		char c;
		while(1) {
			c = _index[i];
			if(c == ':') flag = 1;
			else if(c == ' ') {
				if(index > currentIdx) {
					preLoc = currentLoc;
					currentLoc = i + 1;
					stringLoc += currentSize;
					flag = 0;
					preIdx = currentIdx;
					currentIdx = 0;
					preSize = currentSize;
					currentSize = 0;
				}
				else if(index < currentIdx) {
					break;
				}
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
		if(i && flag && preIdx + preSize + size >= currentIdx) {
			_index.erase(preLoc, i - preLoc + 1);
			newLoc = preLoc;
			newIdx = preIdx;
			newSize = max(max(preIdx + preSize, index + size), currentIdx + currentSize) - newIdx;
			rightGap = size - (currentIdx - index);
			leftGap = preSize - (index - preIdx);
		}
		else if( i && preIdx + preSize >= index) {
			_index.erase(preLoc, currentLoc-preLoc);
			newLoc = preLoc;
			newIdx = preIdx;
			newSize = max(preIdx + preSize, index + size) - newIdx;
			leftGap = preSize - (index - preIdx);
		}
		else if(flag && index + size >= currentIdx) {
			_index.erase(currentLoc, i - currentLoc + 1);
			newLoc = currentLoc;
			newIdx = index;
			newSize = max(index + size, currentIdx + currentSize);
			rightGap = size - (currentIdx - index);
		}
		else {
			newLoc = currentLoc;
			newIdx = index;
			newSize = size;
		}
		_index.insert(newLoc, std::to_string(newIdx) + ":" + std::to_string(newSize) + " ");
		if(assembled == index) {
			_index.erase(0, _index.find(' ') + 1);
			return newSize;
		}
		return 0;
	}

  public:
    //! \brief Construct a `StreamReassembler` that will store up to `capacity` bytes.
    //! \note This capacity limits both the bytes that have been reassembled,
    //! and those that have not yet been reassembled.
    StreamReassembler(const size_t capacity);

    //! \brief Receive a substring and write any newly contiguous bytes into the stream.
    //!
    //! The StreamReassembler will stay within the memory limits of the `capacity`.
    //! Bytes that would exceed the capacity are silently discarded.
    //!
    //! \param data the substring
    //! \param index indicates the index (place in sequence) of the first byte in `data`
    //! \param eof the last byte of `data` will be the last byte in the entire stream
    void push_substring(const std::string &data, const uint64_t index, const bool eof);

    //! \name Access the reassembled byte stream
    //!@{
    const ByteStream &stream_out() const { return _output; }
    ByteStream &stream_out() { return _output; }
    //!@}

    //! The number of bytes in the substrings stored but not yet reassembled
    //!
    //! \note If the byte at a particular index has been pushed more than once, it
    //! should only be counted once for the purpose of this function.
    size_t unassembled_bytes() const;

    //! \brief Is the internal state empty (other than the output stream)?
    //! \returns `true` if no substrings are waiting to be assembled
    bool empty() const;
};

#endif  // SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH
