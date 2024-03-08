#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

using namespace std;

size_t StreamReassembler::update_index(uint64_t index, size_t size) {
    /* pre substring's information */
    uint64_t preIdx = 0;
    size_t preSize = 0;
    size_t preLoc = 0;
    /* post substring's information */
    uint64_t postIdx = 0;
    size_t postSize = 0;
    size_t postLoc = 0;
    /* indicates newly added substring's location */
    size_t myLoc = 0;
    /* initialize members */
    stringLoc = 0;
    rightGap = 0;
    leftGap = 0;
    middleGap = 0;
    /* iterate _index string to find the right place to insert newly added substring's information */
    size_t iter = 0;  // iterator.. index accessor
    while (1) {
        /* get some informations about current substring(post) that iter pointing to */
        postIdx = 0;  // in case of reaching at end '|'
        postSize = 0;
        postLoc = iter;
        if (_index[iter] == '|')
            break;                                            // if it reaches end, then break;
        int f1 = _index.find(':', iter);                      // first index of ':' after iter
        int f2 = _index.find(' ', iter);                      // first index of ' ' after iter
        postIdx = stoi(_index.substr(iter, f1 - iter));       // get a index information about current substring
        postSize = stoi(_index.substr(f1 + 1, f2 - f1 - 1));  // get a size information about current substring
        iter = f2 + 1;                                        // move iterator
        /* decide whether go further or not */
        if (index > postIdx) {  // this means we can go further
            /* update pre substring to point current substring */
            preLoc = postLoc;
            preIdx = postIdx;
            preSize = postSize;
            myLoc = iter;  // also remember this place(in case of overwritting)

            stringLoc += postSize;  // accumulate all size of the passed by substrings
        }
        /* this means current substrings(post) are exist in our newly added substirng
         * so keep going without updating pre substings & myLoc */
        else if (index + size >= postIdx + postSize)
            middleGap += postSize;
        else
            break;  // this means we can't go further
    }
    /* simple variable that reduces our code.. */
    uint64_t newIdx;
    size_t newSize, newLoc, newErase;
    /* decide the type of insertion
     * 1: merging with pre substring and post substring
     * 2: merging with pre substring
     * 3: merging with post substring
     * 4: else (overwritting could be possible) */
    /* if there is pre substring & post substring -> preIdx && postIdx
     * and they are all connected to each other, apply this */
    if (preIdx && postIdx && index + size >= postIdx && preIdx + preSize >= index) {
        newErase = iter - preLoc;
        newLoc = preLoc;
        newIdx = preIdx;
        newSize = max(max(preIdx + preSize, index + size), postIdx + postSize) - newIdx;
        rightGap = size - (postIdx - index);
        leftGap = preSize - (index - preIdx);
    }
    /* if there is pre substring -> preIdx
     * and it is connected to our newly added substring, apply this */
    else if (preIdx && preIdx + preSize >= index) {
        newErase = postLoc - preLoc;
        newLoc = preLoc;
        newIdx = preIdx;
        newSize = max(preIdx + preSize, index + size) - newIdx;
        leftGap = preSize - (index - preIdx);
    }
    /* if there is post substring -> postIdx
     * and it is connected to our newly added substring, apply this */
    else if (postIdx && index + size >= postIdx) {
        newErase = iter - myLoc;
        newLoc = myLoc;
        newIdx = index;
        newSize = max(index + size, postIdx + postSize) - newIdx;
        rightGap = size - (postIdx - index);
    }
    /* else: but must erase some part in case of overwritting */
    else {
        newErase = postLoc - myLoc;
        newLoc = myLoc;
        newIdx = index;
        newSize = size;
    }
    /* erase old information and in that place, insert newly modified information */
    _index.erase(newLoc, newErase);
    _index.insert(newLoc, to_string(newIdx) + ":" + to_string(newSize) + " ");

    /* if this newly added substrings are assembled, then erase from _index string and return the size of assembled
     * strings */
    if (totalRead + assembled == index) {
        _index.erase(0, _index.find(' ') + 1);
        return newSize;
    }
    /* else: normal case return 0 */
    return 0;
}

StreamReassembler::StreamReassembler(const size_t capacity)
    : _output(capacity)
    , _capacity(capacity)
    , totalRead(0)
    , assembled(0)
    , eofFlag(0)
    , _index("|")
    , unassembled("")
    , stringLoc(0)
    , rightGap(0)
    , leftGap(0)
    , middleGap(0) {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    /* update stream in case of reading the bytestream */
    totalRead = _output.bytes_read();
    assembled = _output.buffer_size();

    uint64_t startIdx = totalRead + assembled;  // first index of unassembled scope
    uint64_t endIdx = totalRead + _capacity;    // last index of unassembled scope + 1

    string realData;                         // data that is in boundary of unassembled scope
    uint64_t realStart = index;              // realData's first index
    uint64_t realEnd = index + data.size();  // realData's last index + 1

    /* cutting process (to fit in unassembled scope) */
    if (index >= endIdx)
        return;
    if (index < startIdx)
        realStart = startIdx;
    if (realEnd > endIdx)
        realEnd = endIdx;
    else if (eof)
        eofFlag = 1;  // if receives eof signal and its end index is located with in endIdx, then set eofFlag as 1

    if (realEnd > startIdx) {  // if substrings are already assembled or read, then do nothing. else do below
        size_t realSize = realEnd - realStart;                // realData's size
        realData = data.substr(realStart - index, realSize);  // make cutted data(realData) from original data
        /* update _index string && other members */
        size_t made =
            update_index(realStart, realSize);  // update _index information and get size of newly assembled strings
        /* update unassembled string */
        if (realSize > leftGap + rightGap) {          // if realSize is over Gap(it makes it possible to overwrite)
            unassembled.erase(stringLoc, middleGap);  // erase duplicated strings
            unassembled.insert(
                stringLoc,
                realData.substr(leftGap, realSize - leftGap - rightGap));  // insert realData string(with Gap cutting)
        }
        /* update _output bytestream */
        if (made) {                                      // if there exist newly assembled strings
            _output.write(unassembled.substr(0, made));  // write them in _output bytestream
            unassembled.erase(0, made);                  // and erase them in unassembled string
        }
    }

    /* eof setter */
    if (eofFlag && unassembled.size() == 0)
        _output.end_input();

    return;
}

size_t StreamReassembler::unassembled_bytes() const { return unassembled.size(); }

bool StreamReassembler::empty() const { return _output.buffer_empty() && unassembled.empty(); }
