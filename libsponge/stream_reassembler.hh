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

    uint64_t totalRead;  // total amount of data that is read via _output bytestream
    size_t assembled;    // size of currently assembled but not read data

    bool eofFlag;  // remember whether eof signal is received or not without being discarded.

    std::string _index;  // string that restores pair of information about one substring. (index:size)
    /* _index string
     * this string have (index:size) pairs and each pair is separated by ' ' space character.
     * mark the end of this string with '|'
     * so it is look like this "2:1 4:2 14:1 20:45 |"
     * their indexes are ordered in assending order.
     * if their range conflict each ohter by adding new substring, than merge */
    std::string unassembled;  // string that stores unassembled substrings in assending order by index.(they is no blank
                              // between all of them)
    /* belows are used for modifying unassembled string */
    size_t stringLoc;  // location that new substrings will be added.
    size_t rightGap;   // for overwrite
    size_t leftGap;
    size_t middleGap;

    /* add information about new substring to _index string
     * and return the size of bytes that need to be added to _output bytestream(). */
    size_t update_index(uint64_t index, size_t size);

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
