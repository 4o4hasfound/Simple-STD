#ifndef SSTD_ITERATOR_INCLUDED
#define SSTD_ITERATOR_INCLUDED

#include "core.hpp"

SSTD_BEGIN

template<typename T>
struct random_access_iterator {
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T*;  // or also value_type*
    using reference = T&;  // or also value_type&
};
template<typename T>
struct const_random_access_iterator {
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T*;
    using pointer = T**;  // or also value_type*
    using reference = T*&;  // or also value_type&
};

template<typename T>
struct forward_iterator{
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T*;  // or also value_type*
    using reference = T&;  // or also value_type&
};
template<typename T>
struct const_forward_iterator {
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T*;
    using pointer = T**;  // or also value_type*
    using reference = T*&;  // or also value_type&
};

template<typename T>
struct input_iterator {
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T*;  // or also value_type*
    using reference = T&;  // or also value_type&
};
template<typename T>
struct const_input_iterator {
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T*;
    using pointer = T**;  // or also value_type*
    using reference = T*&;  // or also value_type&
};
SSTD_END

#endif