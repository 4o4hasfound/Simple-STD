#ifndef SSTD_ARRAY_INCLUDED
#define SST_ARRAY_INCLUDED

#include "core.hpp"
#include "Iterator.hpp"

#include <utility>
#include <bitset>
#include <initializer_list>

SSTD_BEGIN

template<typename T, sizet _Count>
class _Array_Iterator;
template<typename T, sizet _Count>
class _Array_Reverse_Iterator;
template<typename T, sizet _Count>
class _Array_Const_Iterator;
template<typename T, sizet _Count>
class _Array_Const_Reverse_Iterator;

// Nothing special, just a normal Array

template<typename T, sizet _Count>
class Array {
	SSTD_STATIC_ASSERT(_Count > 0);
public:
	using iterator = _Array_Iterator<T, _Count>;
	using reverse_iterator = _Array_Reverse_Iterator<T, _Count>;
	using const_iterator = _Array_Const_Iterator<T, _Count>;
	using const_reverse_iterator = _Array_Const_Reverse_Iterator<T, _Count>;
public:
	// Default constructor
	Array() {
		_Fill_Range(0, _Count);
	};

	// Constructor that initialize using a initializer list
	Array(std::initializer_list<T> _List) {
		_Check_Range(_List.size()-1);
		_Fill_Range_Iter(0, _List.begin(), _List.end());
	}

	// Constructor that set all the elements to _Val
	SSTD_EXPLICIT Array(const T& _Val) {
		std::fill(m_data, m_data + _Count, _Val);
	}

	// Destructor
	~Array() {

	}

	SSTD_INLINE SSTD_CONSTEXPR sizet size() const noexcept {
		return _Count;
	}

	// Clear the array. Aka call the destructor of every object, and reconstruct them
	SSTD_INLINE void clear() {
		for (sizet i = 0; i < _Count; ++i) {
			m_data[i].~T();
			new (&m_data[i]) T();
		}
	}

	SSTD_INLINE T& operator[](const sizet& key) noexcept {
		return m_data[key];
	}

	SSTD_INLINE const T& operator[](const sizet& key) const noexcept {
		return m_data[key];
	}

	SSTD_INLINE SSTD_CONSTEXPR T& at(const sizet& key) {
		_Check_Range(key);

		return m_data[key];
	}

	SSTD_INLINE SSTD_CONSTEXPR const T& at(const sizet& key) const {
		_Check_Range(key);

		return m_data[key];
	}

	SSTD_INLINE T& front() noexcept {
		return m_data[0];
	}
	SSTD_INLINE T& back() noexcept {
		return m_data[_Count - 1];
	}
	SSTD_INLINE const T& front() const noexcept {
		return m_data[0];
	}
	SSTD_INLINE const T& back() const noexcept {
		return m_data[_Count - 1];
	}
	
	SSTD_INLINE SSTD_CONSTEXPR const T* data() const noexcept {
		return m_data;
	}

	SSTD_INLINE SSTD_CONSTEXPR iterator begin() noexcept {
		return iterator(this, 0);
	}
	SSTD_INLINE SSTD_CONSTEXPR iterator end() noexcept {
		return iterator(this, _Count);
	}

	SSTD_INLINE SSTD_CONSTEXPR const_iterator begin() const noexcept {
		return const_iterator(this, 0);
	}
	SSTD_INLINE SSTD_CONSTEXPR const_iterator end() const noexcept {
		return const_iterator(this, _Count);
	}

	SSTD_INLINE SSTD_CONSTEXPR reverse_iterator rbegin() noexcept {
		return reverse_iterator(this, _Count-1);
	}
	SSTD_INLINE SSTD_CONSTEXPR reverse_iterator rend() noexcept {
		return reverse_iterator(this, -1);
	}

	SSTD_INLINE SSTD_CONSTEXPR const_reverse_iterator rbegin() const noexcept {
		return const_reverse_iterator(this, _Count - 1);
	}
	SSTD_INLINE SSTD_CONSTEXPR const_reverse_iterator rend() const noexcept {
		return const_reverse_iterator(this, -1);
	}

	SSTD_INLINE SSTD_CONSTEXPR const_iterator cbegin() const noexcept {
		return const_iterator(this, 0);
	}
	SSTD_INLINE SSTD_CONSTEXPR const_iterator cend() const noexcept {
		return const_iterator(this, _Count);
	}

	SSTD_INLINE SSTD_CONSTEXPR const_reverse_iterator crbegin() const noexcept {
		return const_reverse_iterator(this, _Count - 1);
	}
	SSTD_INLINE SSTD_CONSTEXPR const_iterator crend() const noexcept {
		return const_reverse_iterator(this, -1);
	}
private:
	T m_data[_Count];

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
		for (; pos < _Count && _Start != _End; ++pos, ++_Start) {
			m_data[pos] = *_Start;
		}
	}

	SSTD_INLINE void _Check_Range(sizet ind) const {
		if (ind < 0 || ind >= _Count) {
			throw std::out_of_range("Array subscript out of range");
		}
	}
};

template<typename T, sizet _Count>
class _Array_Iterator : public random_access_iterator<T> {
	friend class Array<T, _Count>;
public:
	_Array_Iterator(Array<T, _Count>* arr, sizet ind) :
		m_array(arr), m_ind(ind) {

	}

	SSTD_INLINE _Array_Iterator& operator++() noexcept {
		this->m_ind++;
		return *this;
	}
	SSTD_INLINE _Array_Iterator operator++(int) noexcept {
		_Array_Iterator tmp = *this;
		this->m_ind++;
		return tmp;
	}
	SSTD_INLINE _Array_Iterator& operator--() noexcept {
		this->m_ind--;
		return *this;
	}
	SSTD_INLINE _Array_Iterator operator--(int) noexcept {
		_Array_Iterator tmp = *this;
		this->m_ind--;
		return tmp;
	}

	SSTD_INLINE _Array_Iterator& operator+=(const sizet dis) noexcept {
		this->m_ind += dis;
		return *this;
	}
	SSTD_INLINE _Array_Iterator operator+(const sizet dis) const noexcept {
		return _Array_Iterator(m_array, m_ind + dis);
	}
	SSTD_INLINE _Array_Iterator& operator-=(const sizet dis) noexcept {
		this->m_ind -= dis;
		return *this;
	}
	SSTD_INLINE _Array_Iterator operator-(const sizet dis) const noexcept {
		return _Array_Iterator(m_array, m_ind - dis);
	}

	SSTD_INLINE sizet operator-(const _Array_Iterator dis) const noexcept {
		return m_ind - dis.m_ind;
	}

	SSTD_INLINE T& operator*() const noexcept {
		return this->m_array->operator[](m_ind);
	}
	SSTD_INLINE T& operator->() const noexcept {
		return &this->m_array->operator[](m_ind);
	}

	SSTD_INLINE bool operator==(const _Array_Iterator& other) const noexcept {
		return this->m_array == other.m_array && this->m_ind == other.m_ind;
	}

	SSTD_INLINE bool operator!=(const _Array_Iterator& other) const noexcept {
		return this->m_array != other.m_array || this->m_ind != other.m_ind;
	}
private:
	Array<T, _Count>* m_array;
	sizet m_ind;
};

// -----------------------------------------
//
//   Reverse Iterator
//
// -----------------------------------------

template<typename T, sizet _Count>
class _Array_Reverse_Iterator : public random_access_iterator<T> {
	friend class Array<T, _Count>;
public:
	_Array_Reverse_Iterator(Array<T, _Count>* arr, sizet ind) :
		m_array(arr), m_ind(ind) {

	}

	SSTD_INLINE _Array_Reverse_Iterator& operator++() noexcept {
		this->m_ind--;
		return *this;
	}
	SSTD_INLINE _Array_Reverse_Iterator operator++(int) noexcept {
		_Array_Reverse_Iterator tmp = *this;
		this->m_ind--;
		return tmp;
	}
	SSTD_INLINE _Array_Reverse_Iterator& operator--() noexcept {
		this->m_ind++;
		return *this;
	}
	SSTD_INLINE _Array_Reverse_Iterator operator--(int) noexcept {
		_Array_Reverse_Iterator tmp = *this;
		this->m_ind++;
		return tmp;
	}

	SSTD_INLINE _Array_Reverse_Iterator& operator+=(const sizet dis) noexcept {
		this->m_ind -= dis;
		return *this;
	}
	SSTD_INLINE _Array_Reverse_Iterator operator+(const sizet dis) const noexcept {
		return _Array_Reverse_Iterator(m_array, m_ind - dis);
	}
	SSTD_INLINE _Array_Reverse_Iterator& operator-=(const sizet dis) noexcept {
		this->m_ind += dis;
		return *this;
	}
	SSTD_INLINE _Array_Reverse_Iterator operator-(const sizet dis) const noexcept {
		return _Array_Reverse_Iterator(m_array, m_ind + dis);
	}

	SSTD_INLINE T& operator*() const noexcept {
		return this->m_array->operator[](m_ind);
	}
	SSTD_INLINE T& operator->() const noexcept {
		return &this->m_array->operator[](m_ind);
	}

	SSTD_INLINE bool operator==(const _Array_Reverse_Iterator& other) const noexcept {
		return this->m_array == other.m_array && this->m_ind == other.m_ind;
	}

	SSTD_INLINE bool operator!=(const _Array_Reverse_Iterator& other) const noexcept {
		return this->m_array != other.m_array || this->m_ind != other.m_ind;
	}
private:
	Array<T, _Count>* m_array;
	sizet m_ind;
};

// -----------------------------------------
//
//   Const Iterator
//
// -----------------------------------------

template<typename T, sizet _Count>
class _Array_Const_Iterator : public const_random_access_iterator<T> {
	friend class Array<T, _Count>;
public:
	_Array_Const_Iterator(const Array<T, _Count>* arr, sizet ind) :
		m_array(arr), m_ind(ind) {

	}

	SSTD_INLINE _Array_Const_Iterator& operator++() noexcept {
		this->m_ind++;
		return *this;
	}
	SSTD_INLINE _Array_Const_Iterator operator++(int) noexcept {
		_Array_Const_Iterator tmp = *this;
		this->m_ind++;
		return tmp;
	}
	SSTD_INLINE _Array_Const_Iterator& operator--() noexcept {
		this->m_ind--;
		return *this;
	}
	SSTD_INLINE _Array_Const_Iterator operator--(int) noexcept {
		_Array_Const_Iterator tmp = *this;
		this->m_ind--;
		return tmp;
	}

	SSTD_INLINE _Array_Const_Iterator& operator+=(const sizet dis) noexcept {
		this->m_ind += dis;
		return *this;
	}
	SSTD_INLINE _Array_Const_Iterator operator+(const sizet dis) const noexcept {
		return _Array_Const_Iterator(m_array, m_ind + dis);
	}
	SSTD_INLINE _Array_Const_Iterator& operator-=(const sizet dis) noexcept {
		this->m_ind -= dis;
		return *this;
	}
	SSTD_INLINE _Array_Const_Iterator operator-(const sizet dis) const noexcept {
		return _Array_Const_Iterator(m_array, m_ind - dis);
	}

	SSTD_INLINE const T& operator*() const noexcept {
		return this->m_array->operator[](m_ind);
	}
	SSTD_INLINE const T& operator->() const noexcept {
		return &this->m_array->operator[](m_ind);
	}

	SSTD_INLINE bool operator==(const _Array_Const_Iterator& other) const noexcept {
		return this->m_array == other.m_array && this->m_ind == other.m_ind;
	}

	SSTD_INLINE bool operator!=(const _Array_Const_Iterator& other) const noexcept {
		return this->m_array != other.m_array || this->m_ind != other.m_ind;
	}
private:
	const Array<T, _Count>* m_array;
	sizet m_ind;
};

// -----------------------------------------
//
//   Const Reverse Iterator
//
// -----------------------------------------

template<typename T, sizet _Count>
class _Array_Const_Reverse_Iterator : public const_random_access_iterator<T> {
	friend class Array<T, _Count>;
public:
	_Array_Const_Reverse_Iterator(const Array<T, _Count>* arr, sizet ind) :
		m_array(arr), m_ind(ind) {

	}

	SSTD_INLINE _Array_Const_Reverse_Iterator& operator++() noexcept {
		this->m_ind--;
		return *this;
	}
	SSTD_INLINE _Array_Const_Reverse_Iterator operator++(int) noexcept {
		_Array_Const_Reverse_Iterator tmp = *this;
		this->m_ind--;
		return tmp;
	}
	SSTD_INLINE _Array_Const_Reverse_Iterator& operator--() noexcept {
		this->m_ind++;
		return *this;
	}
	SSTD_INLINE _Array_Const_Reverse_Iterator operator--(int) noexcept {
		_Array_Const_Reverse_Iterator tmp = *this;
		this->m_ind++;
		return tmp;
	}

	SSTD_INLINE _Array_Const_Reverse_Iterator& operator+=(const sizet dis) noexcept {
		this->m_ind -= dis;
		return *this;
	}
	SSTD_INLINE _Array_Const_Reverse_Iterator operator+(const sizet dis) const noexcept {
		return _Array_Const_Reverse_Iterator(m_array, m_ind - dis);
	}
	SSTD_INLINE _Array_Const_Reverse_Iterator& operator-=(const sizet dis) noexcept {
		this->m_ind += dis;
		return *this;
	}
	SSTD_INLINE _Array_Const_Reverse_Iterator operator-(const sizet dis) const noexcept {
		return _Array_Const_Reverse_Iterator(m_array, m_ind + dis);
	}

	SSTD_INLINE const T& operator*() const noexcept {
		return this->m_array->operator[](m_ind);
	}
	SSTD_INLINE const T& operator->() const noexcept {
		return &this->m_array->operator[](m_ind);
	}

	SSTD_INLINE bool operator==(const _Array_Const_Reverse_Iterator& other) const noexcept {
		return this->m_array == other.m_array && this->m_ind == other.m_ind;
	}

	SSTD_INLINE bool operator!=(const _Array_Const_Reverse_Iterator& other) const noexcept {
		return this->m_array != other.m_array || this->m_ind != other.m_ind;
	}
private:
	const Array<T, _Count>* m_array;
	sizet m_ind;
};

SSTD_END
#endif