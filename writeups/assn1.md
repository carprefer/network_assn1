Assignment 1 Writeup
=============

My name: [Sunho Cha]

My POVIS ID: [carprefer]

My student ID (numeric): [20200120]

This assignment took me about [28] hours to do (including the time on studying, designing, and writing the code).

Program Structure and Design of the StreamReassembler:
- StreamReassembler class -
- private member  					description
=========================================================================================================
	| ByteStream _output		| reassembled in-order byte stream	
	| std::string _index		| dictionary string about unassembled string's information
	| std::string unassembled	| unassembled in-order byte string
	=====================================================================================================
	| size_t _capacity			| total capacity of reassembler
	| uint64_t totalRead		| read data so far
	| size_t assembled			| size of currently assembled but not read data
	| bool eofFlag				| remember whether eof siganl is received or not
	| size_t stringLoc			| location that new substrings will be added
	| size_t rightGap...		| information about overlap
    =====================================================================================================
	| size_t update_index()		| function that update _index & return size of newly assembled string
==========================================================================================================
- public memeber					description
==========================================================================================================
	| void push_substring()		| function that add substrings on unassembled and possibly update _output
	| ByteStream &stream_out()	| return _output
	| size_t unassembled_bytes()| return size of unassembled string
	| bool empty()				| return whether reassember is empty or not
==========================================================================================================
- [_index] string: consists of pair of information. 
	- strutcture: 	"(index):(size) (index):(size) ... (index):(size) |"
	- example:		"2:1 5:2 8:3 16:5 |"
	- description: each pair indicates substring's index and its size
	- benefit: we can use it as a parser in unassembled string(it has no blank but we can split it with this _index string)
- [unassembled] string: it piles up newly adding substrings without blank. but order(index) is matter. so 'stringLoc' is used to identify adding location. when the front of unassembled are assembled(know via return value of update_index()), carry them into _output bytestream.

Implementation Challenges:
- It was hard to implementing update_index() function. This function find the right location in which newly adding substring will be inserted. But there are many edge cases like overlapping problem. And I spend hard time to fix this minor errors caused by this function. 
- And I stuck with very trivial problems like replacing '&&' with '&'. this takes a lot of time.

Remaining Bugs: 
- nothing but exist some unhandled edge cases

unhandled edge cases: 
- different strings in the same index
- wrong index

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this assignment better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [dqescribe]
