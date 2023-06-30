#ifndef SSTD_VECTOR_INCLUDED
#define SSTD_VECTOR_INCLUDED

#include "core.hpp"
#include "Iterator.hpp"
#include "Debug/Debug.hpp"

#include <initializer_list>
#include <utility>
#include <stdlib.h>
#include <malloc.h>
#include <algorithm>
#include <xmemory>
#include <new>
#include <utility>

SSTD_BEGIN

template<typename T>
class _Vector_Iterator;
template<typename T>
class _Vector_Reverse_Iterator;
template<typename T>
class _Vector_Const_Iterator;
template<typename T>
class _Vector_Const_Reverse_Iterator;

// This vector clone made a little change in the way it allocates memory
// The std::vector uses new / delete aka the c++ allocator way to allocate memory
// This sstd::vector uses malloc / realloc to get that sweet performance buff
// 
// This sstd::vector has about three times the speed of std::vector without reserve
// And about two times the speed with reserve

template<typename T>
class vector {
public:
	using iterator = _Vector_Iterator<T>;
	using reverse_iterator = _Vector_Reverse_Iterator<T>;
	using const_iterator = _Vector_Const_Iterator<T>;
	using const_reverse_iterator = _Vector_Const_Reverse_Iterator<T>;

public:

	// Default Constructor
	vector() SSTD_DEFAULT;

	// Constructor that initialize 'length' amount of objects 
	SSTD_EXPLICIT vector(sizet length) :
		m_size(length), m_capacity(length), m_data((T*)malloc(sizeof(T)* length)) {
		_Fill_Range(0, length);
	}

	// Constructor that set all the object to val
	vector(sizet length, const T& val) :
		m_size(length), m_capacity(length), m_data((T*)malloc(sizeof(T)* length)) {
		_Fill_Range(0, length, val);
	}

	// Constructor that initialize using a initializer list
	vector(std::initializer_list<T> list) :
		m_size(list.size()), m_capacity(list.size()), m_data((T*)malloc(sizeof(T)* list.size())) {
		_Fill_Range_Iter(0, list.begin(), list.end());
	}

	// Destructor
	~vector() {
		if (m_data) {
			if (std::is_destructible<T>::value) {
				for (int i = 0; i < m_size; ++i) {
					m_data[i].~T();
				}
				free(m_data);
				m_data = nullptr;
			}
		}
	}

	// Clear the vector. Aka call the destructor of every object and free the memory
	SSTD_INLINE void clear() noexcept {
		if (m_size) {
			for (int i = 0; i < m_size; ++i) {
				m_data[i].~T();
			}
		}
		if (m_data != nullptr) {
			free(m_data);
			m_data = nullptr;
		}
		m_size = 0;
		m_capacity = 0;
	}

	// Construct the object to the back of the vector.
	// By constructing it direclty into the memory, 
	// we don't need to call any move / copy constructor.
	template<typename ... _Val>
	SSTD_INLINE void emplace_back(_Val&& ...val) {
		if (m_data == nullptr) {
			_Malloc_Data(8);
		}
		else if (m_size >= m_capacity) {
			_Realloc_Data(m_capacity * 2);
		}
		new (&m_data[m_size++]) T(std::forward<_Val>(val)...);
	}

	// Push val to the back of the vector
	SSTD_INLINE void push_back(const T& val) {
		emplace_back(val);
	}
	// Push val to the back of the vector
	SSTD_INLINE void push_back(T&& val) {
		emplace_back(std::forward<T>(val));
	}

	// Reserve a certain amount of memory ( without initialization )
	SSTD_INLINE void reserve(sizet new_cap) {
		m_capacity += new_cap;
		if (m_data == nullptr) {
			_Malloc_Data(m_capacity);
			return;
		}
		_Realloc_Data(m_capacity);
	}

	// Resize the vector to new_size ( with initialization )
	SSTD_INLINE void resize(sizet new_size) {
		if (m_data == nullptr) {
			_Malloc_Data(new_size);
		}
		else if (new_size > m_capacity) {
			_Realloc_Data(new_size);
		}
		_Fill_Range(m_size, new_size);
		m_size = new_size;
	}

	// Insert the objects in the iterator to pos
	template<typename _Iter>
	SSTD_INLINE void insert(sizet pos, _Iter iter_beg, _Iter iter_end) {
		_Insert_At(pos, iter_beg, iter_end);
	}

	// Insert the objects in the iterator to pos
	template<typename _Iter>
	SSTD_INLINE void insert(iterator pos, _Iter iter_beg, _Iter iter_end) {
		_Insert_At(pos.m_ind, iter_beg, iter_end);
	}

	// Insert the objects in the iterator to pos
	template<typename _Iter>
	SSTD_INLINE void insert(reverse_iterator pos, _Iter iter_beg, _Iter iter_end) {
		_Insert_At(pos.m_ind, iter_beg, iter_end);
	}

	// Insert the initializer_list in the iterator to pos
	SSTD_INLINE void insert(sizet pos, std::initializer_list<T> _list) {
		_Insert_At(pos, _list.begin(), _list.end());
	}

	// Insert the initializer_list in the iterator to pos
	SSTD_INLINE void insert(iterator pos, std::initializer_list<T> _list) {
		_Insert_At(pos.m_ind, _list.begin(), _list.end());
	}

	// Insert the initializer_list in the iterator to pos
	SSTD_INLINE void insert(reverse_iterator pos, std::initializer_list<T> _list) {
		_Insert_At(pos.m_ind, _list.begin(), _list.end());
	}

	SSTD_INLINE void erase(const sizet& pos) {
		_Erase(pos);
	}
	SSTD_INLINE void erase(const sizet& _start, const sizet& _end) {
		_Erase_Range(_start, _end);
	}
	template<typename _Iter>
	SSTD_INLINE void erase(_Iter pos) {
		_Erase(pos.m_ind);
	}
	template<typename _Iter>
	SSTD_INLINE void erase(_Iter _start, _Iter _end) {
		_Erase_Range(_start.m_ind, _end.m_ind);
	}

	SSTD_INLINE SSTD_CONSTEXPR sizet size() const {
		return m_size;
	}

	SSTD_INLINE SSTD_CONSTEXPR sizet capacity() const {
		return m_capacity;
	}

	SSTD_INLINE SSTD_CONSTEXPR T& front() noexcept {
		return m_data[0];
	}
	SSTD_INLINE SSTD_CONSTEXPR T& back() noexcept {
		return m_data[m_size-1];
	}
	SSTD_INLINE SSTD_CONSTEXPR const T& front() const noexcept {
		return m_data[0];
	}
	SSTD_INLINE SSTD_CONSTEXPR const T& back() const noexcept {
		return m_data[m_size - 1];
	}

	SSTD_INLINE SSTD_CONSTEXPR T* data() const noexcept {
		return m_data;
	}

	SSTD_INLINE SSTD_CONSTEXPR T& at(const sizet& key) {
		_Check_Range(key);
		return this->m_data[key];
	}

	SSTD_INLINE SSTD_CONSTEXPR const T& at(const sizet& key) const {
		_Check_Range(key);
		return this->m_data[key];
	}

	SSTD_INLINE SSTD_CONSTEXPR T& operator[](sizet key) noexcept {
		return this->m_data[key];
	}

	SSTD_INLINE SSTD_CONSTEXPR const T& operator[](sizet key) const noexcept {
		return this->m_data[key];
	}

	SSTD_INLINE SSTD_CONSTEXPR iterator begin() {
		return iterator(this, 0);
	}
	SSTD_INLINE SSTD_CONSTEXPR iterator end() {
		return iterator(this, this->m_size);
	}

	SSTD_INLINE SSTD_CONSTEXPR const_iterator begin() const {
		return const_iterator(this, 0);
	}
	SSTD_INLINE SSTD_CONSTEXPR const_iterator end() const {
		return const_iterator(this, this->m_size);
	}

	SSTD_INLINE SSTD_CONSTEXPR reverse_iterator rbegin() {
		return reverse_iterator(this, this->m_size-1);
	}
	SSTD_INLINE SSTD_CONSTEXPR reverse_iterator rend() {
		return reverse_iterator(this, -1);
	}

	SSTD_INLINE SSTD_CONSTEXPR const_reverse_iterator rbegin() const {
		return const_reverse_iterator(this, this->m_size - 1);
	}
	SSTD_INLINE SSTD_CONSTEXPR const_reverse_iterator rend() const {
		return const_reverse_iterator(this, -1);
	}

	SSTD_INLINE SSTD_CONSTEXPR const_iterator cbegin() const {
		return const_iterator(this, 0);
	}
	SSTD_INLINE SSTD_CONSTEXPR const_iterator cend() const {
		return const_iterator(this, this->m_size);
	}

	SSTD_INLINE SSTD_CONSTEXPR const_reverse_iterator crbegin() const {
		return const_reverse_iterator(this, this->m_size - 1);
	}
	SSTD_INLINE SSTD_CONSTEXPR const_reverse_iterator crend() const {
		return const_reverse_iterator(this, -1);
	}
private:
	T* m_data = nullptr;
	sizet m_size = 0;
	sizet m_capacity = 0;

	SSTD_INLINE void _Malloc_Data(sizet memsize) {
		m_data = (T*)malloc(sizeof(T) * memsize);
		m_capacity = memsize;
	}

	// Reallocate memory can help improve performance
	// because instead of allocating new chunks of memory everytime,
	// it extends the current allocated memory, 
	// ( Allocate another chunk of memory if extension is not possible.
	SSTD_INLINE void _Realloc_Data(sizet memsize) {
		T* tmp = (T*)realloc(m_data, sizeof(T) * memsize);

		// Handle situations if there aren't enough memory to extend
		if (tmp == nullptr) {
			tmp = (T*)malloc(sizeof(T) * memsize);
			std::copy(m_data, m_data + m_capacity, tmp);

			// Has already copyed the old data to the new memory, so the old memory is useless
			free(m_data);
		}
		m_data = tmp;
		m_capacity = memsize;
	}

	SSTD_INLINE void _Fill_Range(sizet start, sizet end) {
		for (; start < end; ++start) {
			new (&m_data[start]) T();
		}
	}

	SSTD_INLINE void _Fill_Range(sizet start, sizet end, const T& val) {
		for (; start < end; ++start) {
			m_data[start] = val;
		}
	}

	template<typename _Iter>
	SSTD_INLINE void _Fill_Range_Iter(sizet pos, _Iter _Start, _Iter _End) {
		for (; _Start != _End; ++pos, ++_Start) {
			m_data[pos] = *_Start;
		}
	}

	// Insert objects at a certain position.
	// First move all the existing object that will be affected 
	// Then fill in the iterator
	template<typename _Iter>
	SSTD_INLINE void _Insert_At(sizet pos, _Iter iter_beg, _Iter iter_end) {
		const sizet Dis = iter_end - iter_beg;
		const sizet Total_Cap = m_size + Dis;
		if (Total_Cap > m_capacity) {
			// Allocate more space
			_Realloc_Data(Total_Cap);
		}
		if (pos > m_size) {
			// completly out side the 'insertable range'
			throw std::out_of_range("Invalid insert position");
			return;
		}
		if (pos < m_size) {
			if (false && std::is_trivially_move_constructible<T>::value) {
				std::memmove(m_data + pos + Dis, m_data + pos, sizeof(T) * (m_size - pos));
			}
			else {
				for (int i = pos; i < m_size; ++i) {
					new (m_data + Dis + i) T(std::move(m_data[i]));
				}
			}
		}
		m_size += Dis;
		_Fill_Range_Iter(pos, iter_beg, iter_end);
	}

	SSTD_INLINE void _Erase(const sizet& ind) {
		// Destruct if possible
		if (std::is_destructible<T>::value) {
			m_data[ind].~T();
		}
		// Just override it
		if (std::is_trivially_move_constructible<T>::value) {
			std::memmove(m_data + ind, m_data + ind + 1, sizeof(T) * (m_size - ind - 1));
		}
		else {
			for (int i = ind; i < m_capacity; ++i) {
				new (m_data + i) T(std::move(m_data[i + 1]));
			}
		}
		--m_size;
	}

	SSTD_INLINE void _Erase_Range(const sizet& _start, const sizet& _end) {
		// Destruct if possible
		for (sizet i = _start; i < _end; ++i) {
			if (std::is_destructible<T>::value) {
				m_data[i].~T();
			}
		}
		// Just override it
		if (std::is_trivially_move_constructible<T>::value) {
			std::memmove(m_data + _start, m_data + _end, sizeof(T) * (m_size - _end));
		}
		else {
			const sizet Dis = _start - _end;
			for (int i = _start; i < _end; ++i) {
				new (m_data + i) T(std::move(m_data[i + Dis]));
			}
		}
		m_size-=_end - _start;
	}

	SSTD_INLINE void _Check_Range(const sizet& ind) const {
		if (ind < 0 || ind >= m_size) {
			throw std::out_of_range("Vector subscript out of range");
		}
	}
	
};

// -----------------------------------------
//
//   Random access Iterator
//
// -----------------------------------------

template<typename T>
class _Vector_Iterator : public random_access_iterator<T> {
	friend class vector<T>;
public:
	_Vector_Iterator(vector<T>* vec, sizet ind) :
		m_vec(vec), m_ind(ind) {

	}

	SSTD_INLINE _Vector_Iterator& operator++() noexcept {
		this->m_ind++;
		return *this;
	}
	SSTD_INLINE _Vector_Iterator operator++(int) noexcept {
		_Vector_Iterator tmp = *this;
		this->m_ind++;
		return tmp;
	}
	SSTD_INLINE _Vector_Iterator& operator--() noexcept {
		this->m_ind--;
		return *this;
	}
	SSTD_INLINE _Vector_Iterator operator--(int) noexcept {
		_Vector_Iterator tmp = *this;
		this->m_ind--;
		return tmp;
	}

	SSTD_INLINE _Vector_Iterator& operator+=(const sizet dis) noexcept {
		this->m_ind += dis;
		return *this;
	}
	SSTD_INLINE _Vector_Iterator operator+(const sizet dis) const noexcept {
		return _Vector_Iterator(m_vec, m_ind + dis);
	}
	SSTD_INLINE _Vector_Iterator& operator-=(const sizet dis) noexcept {
		this->m_ind -= dis;
		return *this;
	}
	SSTD_INLINE _Vector_Iterator operator-(const sizet dis) const noexcept {
		return _Vector_Iterator(m_vec, m_ind - dis);
	}

	SSTD_INLINE sizet operator-(const _Vector_Iterator dis) const noexcept {
		return m_ind - dis.m_ind;
	}

	SSTD_INLINE T& operator*() const noexcept {
		return this->m_vec->operator[](m_ind);
	}
	SSTD_INLINE T& operator->() const noexcept {
		return &this->m_vec->operator[](m_ind);
	}

	SSTD_INLINE bool operator==(const _Vector_Iterator& other) const noexcept {
		return this->m_vec == other.m_vec && this->m_ind == other.m_ind;
	}

	SSTD_INLINE bool operator!=(const _Vector_Iterator& other) const noexcept {
		return this->m_vec != other.m_vec || this->m_ind != other.m_ind;
	}
private:
	vector<T>* m_vec;
	sizet m_ind;
};

// -----------------------------------------
//
//   Reverse Random access Iterator
//
// -----------------------------------------

template<typename T>
class _Vector_Reverse_Iterator: public random_access_iterator<T> {
	friend class vector<T>;
public:
	_Vector_Reverse_Iterator(vector<T>* vec, sizet ind) :
		m_vec(vec), m_ind(ind) {

	}

	SSTD_INLINE _Vector_Reverse_Iterator& operator++() noexcept {
		this->m_ind--;
		return *this;
	}
	SSTD_INLINE _Vector_Reverse_Iterator operator++(int) noexcept {
		_Vector_Reverse_Iterator tmp = *this;
		this->m_ind--;
		return tmp;
	}
	SSTD_INLINE _Vector_Reverse_Iterator& operator--() noexcept {
		this->m_ind++;
		return *this;
	}
	SSTD_INLINE _Vector_Reverse_Iterator operator--(int) noexcept {
		_Vector_Reverse_Iterator tmp = *this;
		this->m_ind++;
		return tmp;
	}

	SSTD_INLINE _Vector_Reverse_Iterator& operator+=(const sizet dis) noexcept {
		this->m_ind -= dis;
		return *this;
	}
	SSTD_INLINE _Vector_Reverse_Iterator operator+(const sizet dis) const noexcept {
		return _Vector_Reverse_Iterator(m_vec, m_ind - dis);
	}
	SSTD_INLINE _Vector_Reverse_Iterator& operator-=(const sizet dis) noexcept {
		this->m_ind += dis;
		return *this;
	}
	SSTD_INLINE _Vector_Reverse_Iterator operator-(const sizet dis) const noexcept {
		return _Vector_Reverse_Iterator(m_vec, m_ind + dis);
	}

	SSTD_INLINE T& operator*() const noexcept {
		return this->m_vec->operator[](m_ind);
	}
	SSTD_INLINE T& operator->() const noexcept {
		return &this->m_vec->operator[](m_ind);
	}

	SSTD_INLINE bool operator==(const _Vector_Reverse_Iterator& other) const noexcept {
		return this->m_vec == other.m_vec && this->m_ind == other.m_ind;
	}

	SSTD_INLINE bool operator!=(const _Vector_Reverse_Iterator& other) const noexcept {
		return this->m_vec != other.m_vec || this->m_ind != other.m_ind;
	}
private:
	vector<T>* m_vec;
	sizet m_ind;
};

// -----------------------------------------
//
//   Const Random access Iterator
//
// -----------------------------------------

template<typename T>
class _Vector_Const_Iterator: public const_random_access_iterator<T> {
	friend class vector<T>;
public:
	_Vector_Const_Iterator(const vector<T>* vec, sizet ind) :
		m_vec(vec), m_ind(ind) {

	}

	SSTD_INLINE _Vector_Const_Iterator& operator++() noexcept {
		this->m_ind++;
		return *this;
	}
	SSTD_INLINE _Vector_Const_Iterator operator++(int) noexcept {
		_Vector_Const_Iterator tmp = *this;
		this->m_ind++;
		return tmp;
	}
	SSTD_INLINE _Vector_Const_Iterator& operator--() noexcept {
		this->m_ind--;
		return *this;
	}
	SSTD_INLINE _Vector_Const_Iterator operator--(int) noexcept {
		_Vector_Const_Iterator tmp = *this;
		this->m_ind--;
		return tmp;
	}

	SSTD_INLINE _Vector_Const_Iterator& operator+=(const sizet dis) noexcept {
		this->m_ind += dis;
		return *this;
	}
	SSTD_INLINE _Vector_Const_Iterator operator+(const sizet dis) const noexcept {
		return _Vector_Const_Iterator(m_vec, m_ind + dis);
	}
	SSTD_INLINE _Vector_Const_Iterator& operator-=(const sizet dis) noexcept {
		this->m_ind -= dis;
		return *this;
	}
	SSTD_INLINE _Vector_Const_Iterator operator-(const sizet dis) const noexcept {
		return _Vector_Const_Iterator(m_vec, m_ind - dis);
	}

	SSTD_INLINE const T& operator*() const noexcept {
		return this->m_vec->operator[](m_ind);
	}
	SSTD_INLINE const T& operator->() const noexcept {
		return &this->m_vec->operator[](m_ind);
	}

	SSTD_INLINE bool operator==(const _Vector_Const_Iterator& other) const noexcept {
		return this->m_vec == other.m_vec && this->m_ind == other.m_ind;
	}

	SSTD_INLINE bool operator!=(const _Vector_Const_Iterator& other) const noexcept {
		return this->m_vec != other.m_vec || this->m_ind != other.m_ind;
	}
private:
	const vector<T>* m_vec;
	sizet m_ind;
};

// -----------------------------------------
//
//   Const Reverse Random access Iterator
//
// -----------------------------------------

template<typename T>
class _Vector_Const_Reverse_Iterator : public const_random_access_iterator<T> {
	friend class vector<T>;
public:
	_Vector_Const_Reverse_Iterator(const vector<T>* vec, sizet ind) :
		m_vec(vec), m_ind(ind) {

	}

	SSTD_INLINE _Vector_Const_Reverse_Iterator& operator++() noexcept {
		this->m_ind--;
		return *this;
	}
	SSTD_INLINE _Vector_Const_Reverse_Iterator operator++(int) noexcept {
		_Vector_Const_Reverse_Iterator tmp = *this;
		this->m_ind--;
		return tmp;
	}
	SSTD_INLINE _Vector_Const_Reverse_Iterator& operator--() noexcept {
		this->m_ind++;
		return *this;
	}
	SSTD_INLINE _Vector_Const_Reverse_Iterator operator--(int) noexcept {
		_Vector_Const_Reverse_Iterator tmp = *this;
		this->m_ind++;
		return tmp;
	}

	SSTD_INLINE _Vector_Const_Reverse_Iterator& operator+=(const sizet dis) noexcept {
		this->m_ind -= dis;
		return *this;
	}
	SSTD_INLINE _Vector_Const_Reverse_Iterator operator+(const sizet dis) const noexcept {
		return _Vector_Const_Reverse_Iterator(m_vec, m_ind - dis);
	}
	SSTD_INLINE _Vector_Const_Reverse_Iterator& operator-=(const sizet dis) noexcept {
		this->m_ind += dis;
		return *this;
	}
	SSTD_INLINE _Vector_Const_Reverse_Iterator operator-(const sizet dis) const noexcept {
		return _Vector_Const_Reverse_Iterator(m_vec, m_ind + dis);
	}

	SSTD_INLINE const T& operator*() const noexcept {
		return this->m_vec->operator[](m_ind);
	}
	SSTD_INLINE const T& operator->() const noexcept {
		return &this->m_vec->operator[](m_ind);
	}

	SSTD_INLINE bool operator==(const _Vector_Const_Reverse_Iterator& other) const noexcept {
		return this->m_vec == other.m_vec && this->m_ind == other.m_ind;
	}

	SSTD_INLINE bool operator!=(const _Vector_Const_Reverse_Iterator& other) const noexcept {
		return this->m_vec != other.m_vec || this->m_ind != other.m_ind;
	}
private:
	const vector<T>* m_vec;
	sizet m_ind;
};

SSTD_END

#endif