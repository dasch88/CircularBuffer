/*
 CircularBuffer.h - Circular buffer library for Arduino.
 Copyright (c) 2017 Roberto Lo Giacco.

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as 
 published by the Free Software Foundation, either version 3 of the 
 License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef CIRCULAR_BUFFER_H_
#define CIRCULAR_BUFFER_H_
#include <stdint.h>
#include <stddef.h>

#ifdef CIRCULAR_BUFFER_DEBUG
#include <Print.h>
#endif

#if defined(__AVR__) && !defined(STD_LIB_NEW_OP)
	/**
	 * Overloading for operator new for placement-new syntax.
	 * In standard C++, this is provided by the standard library header <new>. However, AVR-GCC does not provide that header,
	 * so we define our own operator new.
	 * If any other Arduino library does the same, this might result in a compiler error, in which case,
	 * please declare the STD_LIB_NEW_OP switch to skip this.
	 */
	inline void *operator new(size_t, void *buf) { return buf; }
#else
#	include <new>
#endif

namespace Helper {
	template<bool FITS8, bool FITS16> struct Index {
		using Type = uint32_t;
	};

	template<> struct Index<false, true> {
		using Type = uint16_t;
	};

	template<> struct Index<true, true> {
		using Type = uint8_t;
	};
}

template<typename T, size_t S, typename IT = typename Helper::Index<(S <= UINT8_MAX), (S <= UINT16_MAX)>::Type> class CircularBuffer {
public:
	/**
	 * The buffer capacity: read only as it cannot ever change.
	 */
	static constexpr IT capacity = static_cast<IT>(S);

	/**
	 * Aliases the index type, can be used to obtain the right index type with `decltype(buffer)::index_t`.
	 */
	using index_t = IT;

	CircularBuffer();

	/**
	 * Adds an element to the beginning of buffer: the operation returns `false` if the addition caused overwriting an existing element.
	 */
	template<typename W>
	bool unshift(W&& value);

	/**
	 * Adds an element to the end of buffer: the operation returns `false` if the addition caused overwriting an existing element.
	 */
	template<typename W>
	bool push(W&& value);

	/**
	 * Removes an element from the beginning of the buffer.
	 */
	T shift();

	/**
	 * Removes an element from the end of the buffer.
	 */
	T pop();

	/**
	 * Returns the element at the beginning of the buffer.
	 */
	inline T& first();

	/**
	 * Returns the element at the end of the buffer.
	 */
	inline T& last();

	/**
	 * Array-like access to buffer
	 */
	T& operator [] (IT index);

	/**
	 * Returns how many elements are actually stored in the buffer.
	 */
	IT inline size();

	/**
	 * Returns how many elements can be safely pushed into the buffer.
	 */
	IT inline available();

	/**
	 * Returns `true` if no elements can be removed from the buffer.
	 */
	bool inline isEmpty() const;

	/**
	 * Returns `true` if no elements can be added to the buffer without overwriting existing elements.
	 */
	bool inline isFull() const;

	/**
	 * Resets the buffer to a clean status, making all buffer positions available.
	 */
	void inline clear();

	#ifdef CIRCULAR_BUFFER_DEBUG
	void inline debug(Print* out);
	void inline debugFn(Print* out, void (*printFunction)(Print*, const T&));
	#endif

private:
	union Node {
		T value;
		Node() {}
		~Node() {}
	} buffer[S], *head, *tail;
#ifndef CIRCULAR_BUFFER_INT_SAFE
	IT count;
#else
	volatile IT count;
#endif
};

#include <CircularBuffer.tpp>
#endif
