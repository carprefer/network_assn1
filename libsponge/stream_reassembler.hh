#ifndef SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH
#define SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH

#include "byte_stream.hh"

#include <cstdint>
#include <string>
#include <iostream>

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
	size_t middleGap;
	bool eofFlag;
	
	int update_index(uint64_t index, int size) {
		using std::endl;
		using std::cout;
		using namespace std;
		using std::max;
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
	/*	int sum = 0;
		string clone = _index;
		while(1) {
			if(clone[0] == '|') break;
			sum += stoi(clone.substr(clone.find(':')+1, clone.find(" ") -clone.find(':') + 1));
			clone.erase(0, clone.find(' ') + 1);
		}
		cout << "sum / real : " << sum << " " << unassembled.size() << endl;
	*/
		uint64_t newIdx;
		int newSize, newLoc;
		if((preIdx && flag) && (index + size >= currentIdx && preIdx + preSize >= index)) {
			cout << "case1" << endl;
			_index.erase(preLoc, i - preLoc + 1);
			newLoc = preLoc;
			newIdx = preIdx;
			newSize = max(max(preIdx + preSize, index + size), currentIdx + currentSize) - newIdx;
			rightGap = size - (currentIdx - index);
			leftGap = preSize - (index - preIdx);
		}
		else if( preIdx && (preIdx + preSize >= index)) {
			cout << "case2" << endl;
			_index.erase(preLoc, currentLoc-preLoc);
			newLoc = preLoc;
			newIdx = preIdx;
			newSize = max(preIdx + preSize, index + size) - newIdx;
			leftGap = preSize - (index - preIdx);
		}
		else if(flag && (index + size >= currentIdx)) {
			cout << "case3 " << endl;
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
