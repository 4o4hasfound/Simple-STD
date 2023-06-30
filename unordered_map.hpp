#ifndef SSTD_UNORDERED_MAP_INCLUDED
#define SSTD_UNORDERED_MAP_INCLUDED

#include "core.hpp"

#include <cmath>
#include <initializer_list>
#include <utility>
#include <ratio>

SSTD_BEGIN

// -----------------------------------------
//
//   Hash functor
//
// -----------------------------------------

template<typename T>
struct _Deault_Hash{
	SSTD_INLINE SSTD_CONSTEXPR sizet operator()(const T& key) const {
		return std::hash<T>::_Do_hash(key);
	}
};

// -----------------------------------------
//
//   probing functors
//
// -----------------------------------------

template<typename T, typename _Hash>
struct _Linear_Prob {
	SSTD_INLINE sizet operator()(const T& key, const sizet& i, const sizet& m, const _Hash hasher) const {
		return (hasher(key) + i) % m;
	}
};
template<typename T, int32 c1, int32 c2, typename _Hash>
struct _Quadratic_Prob1 {
	SSTD_INLINE sizet operator()(const T& key, const sizet& i, const sizet& m, const _Hash hasher) const {
		return (hasher(key) + c1 * i + c2 * i * i) % m;
	}
};
template<typename T, int32 c1, int32 c2, typename _Hash>
struct _Quadratic_Prob2 {
	SSTD_INLINE sizet operator()(const T& key, const sizet& i, const sizet& m, const _Hash hasher) const {
		return (hasher(key) - pow(-1, i) * (i / 2) * (i / 2)) % m;
	}
};
template<typename T, typename _Hash>
struct _Double_Hash_Prob {
	SSTD_INLINE sizet operator()(const T& key, const sizet& i, const sizet& m, const _Hash hasher) const {
		const sizet _Hash_Res = hasher(key);					  
		return (_Hash_Res + // First hash
			i * ( _Hash_Res & 0xffffffffffffffff // i * second hash
				| 0x0000000000000001 // Add this to make the second hash result an odd number
			) 
		) % m;
	}
};


// -----------------------------------------
//
//   Iterator declarations
//
// -----------------------------------------

template<
	typename _KeyT,
	typename _EltT,
	typename _Hash = _Deault_Hash<_KeyT>,
	typename _ProbT = _Double_Hash_Prob<_KeyT, _Hash>
>
class _Unordered_Map_Iterator;
template<
	typename _KeyT,
	typename _EltT,
	typename _Hash = _Deault_Hash<_KeyT>,
	typename _ProbT = _Double_Hash_Prob<_KeyT, _Hash>
>
class _Unordered_Map_Const_Iterator;

// This is a completly different implementation than the one in std
// std::unordered_map uses close addressing / chaining
// This sstd::unordered_map uses open addressing
// And this makes this sstd::unordered_map about 2 times faster than std::unordered_map 
// when it comes to inserting
// And performs 'slightly' better than std::unordered_map for other operations
//
// The capacity needs to be a power of 2

template<
	typename _KeyT,	// Key type
	typename _EltT,		// Element type
	typename _Hash = _Deault_Hash<_KeyT>, // Hash function 
	typename _ProbT = _Double_Hash_Prob<_KeyT, _Hash> // probing function
> 
class unordered_map {
public:
	friend class _Unordered_Map_Iterator<_KeyT, _EltT, _Hash, _ProbT>;
	friend class _Unordered_Map_Const_Iterator<_KeyT, _EltT, _Hash, _ProbT>;
	using iterator = _Unordered_Map_Iterator<_KeyT, _EltT, _Hash, _ProbT>;
	using const_iterator = _Unordered_Map_Const_Iterator<_KeyT, _EltT, _Hash, _ProbT>;
private:
	struct _Map_Element {
		_KeyT key;
		_EltT elt;
		bool occupied = false;
	};
public:

	// Default constructor
	unordered_map() {
		_Malloc_Table(8); // just some random magic number
	};

	// Constructor that initialize using a initializer list
	// std::pair(Key, Element)
	unordered_map(std::initializer_list<std::pair<_KeyT, _EltT> > list) {
		// How many space does one element take up, if not exceeding the max_load_factor
		const Decimal additional_size = 1.0 / m_max_load_factor;
		const sizet actual_reserved_size = list.size() * additional_size + 1; // +1 just to be safe, you know

		_Malloc_Table(actual_reserved_size);
		_Load_Iterator(list.begin(), list.end());
	}
	
	~unordered_map() {
		// Destruct every destructable value
		for (sizet i = 0; i < m_capacity; ++i) {
			if (m_table[i].occupied) {
				if (std::is_destructible<_KeyT>::value) {
					m_table[i].key.~_KeyT();
				}
				if (std::is_destructible<_EltT>::value) {
					m_table[i].elt.~_EltT();
				}
			}
		}
		free(m_table);
		m_table = nullptr;
		m_capacity = 0;
		m_size = 0;
	}

	// Basically the destructor
	SSTD_INLINE void clear() {
		for (sizet i = 0; i < m_capacity; ++i) {
			if (m_table[i].occupied) {
				if (std::is_destructible<_KeyT>::value) {
					m_table[i].key.~_KeyT();
				}
				if (std::is_destructible<_EltT>::value) {
					m_table[i].elt.~_EltT();
				}
			}
		}
		free(m_table);
		m_table = nullptr;
		m_capacity = 0;
		m_size = 0;
	}

	SSTD_INLINE void insert(const _KeyT& key, const _EltT& elt) {
		_Insert(key, elt); 
		// Not going to do anything to it yet
	}
	SSTD_INLINE void insert(const _KeyT& key, _EltT&& elt) {
		_Insert(key, std::move(elt));
		// Not going to do anything to it yet
	}
	SSTD_INLINE void insert(const std::pair<_KeyT, _EltT>& pair) {
		_Insert(pair.first, pair.second);
		// Not going to do anything to it yet
	}
	SSTD_INLINE void insert(const _KeyT& key) {
		_Insert(key);
		// Not going to do anything to it yet
	}

	SSTD_INLINE void erase(const _KeyT& key) {
		_Erase(key);
	}

	// Malloc / Realloc the additional _size
	SSTD_INLINE void reserve(const sizet& _size) {
		if (m_table == nullptr) {
			_Malloc_Table(_size);
		}
		else {
			_Realloc_Table(m_capacity + _size);
		}
	}

	SSTD_INLINE SSTD_CONSTEXPR sizet size() const noexcept {
		return m_size;
	}
	SSTD_INLINE SSTD_CONSTEXPR sizet capacity() const noexcept {
		return m_capacity;
	}
	SSTD_INLINE SSTD_CONSTEXPR sizet empty() const noexcept {
		return m_size == 0;
	}

	// Construct a empty value into the table if the key doesn't exist
	SSTD_INLINE _EltT& operator[](const _KeyT& key) {
		//_Print();
		iterator ptr = _Search(key);
		if (ptr != end()) {
			return m_table[ptr.m_ind].elt;
		}
		iterator itr = _Insert(key, _EltT());
		return m_table[itr.m_ind].elt;
	}

	// Straight up return
	SSTD_INLINE const _EltT& operator[](const _KeyT& key) const noexcept {
		return m_table[_Search(key).m_ind].elt;
	}

	// Mantain this below max_load_factor
	SSTD_INLINE SSTD_CONSTEXPR Decimal load_factor() const {
		return static_cast<Decimal>(m_size) / (m_capacity ? m_capacity : 1);
	}

	SSTD_INLINE SSTD_CONSTEXPR iterator begin() noexcept {
		return iterator(this, 0);
	}
	SSTD_INLINE SSTD_CONSTEXPR const_iterator begin() const noexcept {
		return const_iterator(this, 0);
	}
	SSTD_INLINE SSTD_CONSTEXPR iterator end() noexcept {
		return iterator(this, m_capacity);
	}
	SSTD_INLINE SSTD_CONSTEXPR const_iterator end() const noexcept {
		return const_iterator(this, m_capacity);
	}

	SSTD_INLINE SSTD_CONSTEXPR iterator cbegin() noexcept {
		return const_iterator(this, 0);
	}
	SSTD_INLINE SSTD_CONSTEXPR iterator cend() noexcept {
		return const_iterator(this, m_capacity);
	}
private:
	_Map_Element* m_table = nullptr;

	const _Hash m_Hasher;
	const _ProbT m_prob;

	sizet m_size = 0;
	sizet m_capacity = 0;

	Decimal m_max_load_factor = 0.5;

	SSTD_INLINE void _Malloc_Table(const sizet& memsize) {
		m_capacity = memsize;
		m_table = (_Map_Element*)malloc(sizeof(_Map_Element) * m_capacity);
		for (sizet i = 0; i < m_capacity; ++i) {
			m_table[i].occupied = false;
		}
	}

	SSTD_INLINE void _Realloc_Table(const sizet& new_size) {
		_Map_Element* tmp = (_Map_Element*)realloc(m_table, sizeof(_Map_Element) * new_size);

		// Handle situations if there aren't enough memory to extend
		if (tmp == nullptr) {
			tmp = (_Map_Element*)malloc(sizeof(_Map_Element) * new_size);
			std::copy(m_table, m_table + m_capacity, tmp);
			// Has already copyed the old data to the new memory, so the old memory is useless
			free(m_table);
		}
		m_table = tmp;
		for (sizet i = m_capacity; i < new_size; ++i) {
			m_table[i].occupied = false;
		}
		m_capacity = new_size;
	}

	template<typename _TE>
	SSTD_INLINE iterator _Insert(const _KeyT& key, _TE&& elt) {
		if (m_table == nullptr) {
			_Malloc_Table(4);
			// Yet another magic number
			// ( Just kidding, set it to 4 so it will not trigger the reallocation until the 3rd insert )
		}
		// Mantain load_factor below the max_load_factor
		if (load_factor() >= m_max_load_factor) {
			_Realloc_Table(m_capacity * 2);
		}
		sizet i = 0;
		while (i != m_capacity) {
			// Get probing index
			const sizet ind = m_prob(key, i, m_capacity, m_Hasher);
			// That block is empty ( is available )
			if (!m_table[ind].occupied) {
				++m_size;
				// Acquire it
				m_table[ind].occupied = true;

				// Construct it
				m_table[ind].key = key;
				new (&m_table[ind].elt) _EltT(elt);

				return iterator(this, ind);
			}
			// Or that block has the same key
			else if (m_table[ind].key == key) {
				// Construct it
				new (&m_table[ind].elt) _EltT(std::move(elt));

				return iterator(this, ind);
			}
			++i;
		}
		// The table is full ( which won't happen, or something is really REALLY wrong)
		return end();
	}

	// Insert default constructor
	SSTD_INLINE iterator _Insert(const _KeyT& key) {
		if (m_table == nullptr) {
			_Malloc_Table(4);
			// Yet another magic number
			// ( Just kidding, set it to 4 so it will not trigger the reallocation until the 3rd insert )
		}
		// Mantain load_factor below the max_load_factor
		if (load_factor() >= m_max_load_factor) {
			_Realloc_Table(m_capacity * 2);
		}
		sizet i = 0;
		while (i != m_capacity) {
			// Get probing index
			const sizet ind = m_prob(key, i, m_capacity, m_Hasher);
			// That block is empty ( is available )
			if (!m_table[ind].occupied) {
				++m_size;
				// Acquire it
				m_table[ind].occupied = true;

				// Construct it
				m_table[ind].key = key;
				new (&m_table[ind].elt) _EltT();

				return iterator(this, ind);
			}
			// Or that block has the same key
			else if (m_table[ind].key == key) {
				// Construct it
				new (&m_table[ind].elt) _EltT();

				return iterator(this, ind);
			}
			++i;
		}
		// The table is full ( which won't happen, or something is really REALLY wrong)
		return end();
	}

	SSTD_INLINE iterator _Search(const _KeyT& key) {
		sizet i = 0;
		while (i != m_capacity) {
			// Get probing index
			const sizet ind = m_prob(key, i, m_capacity, m_Hasher);
			if (m_table[ind].key == key) {
				return iterator(this, ind);
			}
			else if (!m_table[ind].occupied) {
				return end();
			}
			++i;
		}

		// Didn't find
		return end();
	}

	// A const version that returns const_iterator
	SSTD_INLINE const_iterator _Search(const _KeyT key) const {
		sizet i = 0;
		while (i != m_capacity) {
			const sizet ind = m_prob(key, i, m_capacity, m_Hasher);
			if (m_table[ind].key == key) {
				return const_iterator(this, ind);
			}
			else if (!m_table[ind].occupied) {
				return end();
			}
			++i;
		}
		return end();
	}

	// Erase the key
	SSTD_INLINE void _Erase(const _KeyT& key) {
		sizet i = 0;
		while (i != m_capacity) {
			const sizet ind = m_prob(key, i, m_capacity, m_Hasher);
			if (m_table[ind].key == key && m_table[ind].occupied) {
				m_table[ind].occupied = false;
				if (std::is_destructible<_EltT>::value) {
					m_table[ind].elt.~_EltT();
				}
				if (std::is_destructible<_KeyT>::value) {
					m_table[ind].key.~_KeyT();
				}
				--m_size;
				return;
			}
			++i;
		}
	}

	// Load an iterator into the table
	template<typename _Iter>
	SSTD_INLINE void _Load_Iterator(_Iter _Begin, _Iter _End) {
		for (; _Begin != _End; ++_Begin) {
			_Insert(_Begin->first, _Begin->second);
		}
	}
};

// -----------------------------------------
//
//   Forward Iterator
//
// -----------------------------------------

template<
	typename _KeyT,
	typename _EltT,
	typename _Hash,
	typename _ProbT
>
class _Unordered_Map_Iterator : public forward_iterator<std::pair<_KeyT, _EltT> > {
	friend class unordered_map<_KeyT, _EltT, _Hash, _ProbT>;
public:
	_Unordered_Map_Iterator(unordered_map<_KeyT, _EltT, _Hash, _ProbT>* _map, sizet ind) :
		m_map(_map), m_ind(ind) {

	}

	SSTD_INLINE _Unordered_Map_Iterator& operator++() noexcept {
		do { this->m_ind++; } while (m_ind < m_map->m_capacity && m_map->m_table[m_ind].available);
		return *this;
	}
	SSTD_INLINE _Unordered_Map_Iterator operator++(int) noexcept {
		_Unordered_Map_Iterator tmp = *this;
		do { this->m_ind++; } while (m_ind < m_map->m_capacity && !m_map->m_table[m_ind].occupied);
		return tmp;
	}

	SSTD_INLINE std::pair<_KeyT, _EltT> operator*() noexcept {
		return { this->m_map->m_table[m_ind].key, this->m_map->m_table[m_ind].elt };
	}

	SSTD_INLINE bool operator==(const _Unordered_Map_Iterator& other) const noexcept {
		return this->m_map == other.m_map && this->m_ind == other.m_ind;
	}

	SSTD_INLINE bool operator!=(const _Unordered_Map_Iterator& other) const noexcept {
		return this->m_map != other.m_map || this->m_ind != other.m_ind;
	}
private:
	unordered_map<_KeyT, _EltT, _Hash, _ProbT>* m_map;
	sizet m_ind;
};

// -----------------------------------------
//
//   Const Forward Iterator
//
// -----------------------------------------

template<
	typename _KeyT,
	typename _EltT,
	typename _Hash,
	typename _ProbT
>
class _Unordered_Map_Const_Iterator : public const_forward_iterator<std::pair<_KeyT, _EltT>> {
	friend class unordered_map<_KeyT, _EltT, _Hash, _ProbT>;
	friend class _Unordered_Map_Iterator<_KeyT, _EltT, _Hash, _ProbT>;
public:
	_Unordered_Map_Const_Iterator(const unordered_map<_KeyT, _EltT, _Hash, _ProbT>* _map, sizet ind) :
		m_map(_map), m_ind(ind) {

	}
	_Unordered_Map_Const_Iterator(_Unordered_Map_Iterator<_KeyT, _EltT, _Hash, _ProbT> itr) :
		m_map(itr.m_map), m_ind(itr.m_ind) {

	}

	SSTD_INLINE _Unordered_Map_Const_Iterator& operator++() noexcept {
		do { this->m_ind++; } while (m_ind < m_map->m_capacity && m_map->m_table[m_ind].available);
		return *this;
	}
	SSTD_INLINE _Unordered_Map_Const_Iterator operator++(int) noexcept {
		_Unordered_Map_Const_Iterator tmp = *this;
		do { this->m_ind++; } while (m_ind < m_map->m_capacity && !m_map->m_table[m_ind].occupied);
		return tmp;
	}

	SSTD_INLINE std::pair<_KeyT, _EltT> operator*() const noexcept {
		return { this->m_map->m_table[m_ind].key, this->m_map->m_table[m_ind].elt };
	}

	SSTD_INLINE bool operator==(const _Unordered_Map_Const_Iterator& other) const noexcept {
		return this->m_map == other.m_map && this->m_ind == other.m_ind;
	}

	SSTD_INLINE bool operator!=(const _Unordered_Map_Const_Iterator& other) const noexcept {
		return this->m_map != other.m_map || this->m_ind != other.m_ind;
	}
private:
	const unordered_map<_KeyT, _EltT, _Hash, _ProbT>* m_map;
	sizet m_ind;
};

SSTD_END

#endif