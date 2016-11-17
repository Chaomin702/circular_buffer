#pragma once
#include <allocators>
#include <assert.h>
template <typename T>
class circular_buffer {
public:
	enum { default_size = 100 };
	explicit circular_buffer(size_t size = default_size);
	circular_buffer(const circular_buffer<T>& rhs);
	circular_buffer<T>& operator = (circular_buffer<T> rhs) {
		swap(rhs);
		return *this;
	}
	~circular_buffer();
	size_t size()const { return contents_size_; }
	bool empty()const { return contents_size_ == 0; }
	void pop_front();
	void push_back(const T& new_value);
	void swap(const circular_buffer<T>& rhs);
	const T& top()const {
		return array_[head_];
	}
	const T& operator[](size_t index)const {
		assert(index < size());
		return array_[(index + head_) % array_size_];
	}
	T& operator[](size_t index) {
		return const_cast<T&>(const_cast<const circular_buffer<T>*>(this)->operator[](index));
	}
private:
	T* array_;
	T array_size_;
	size_t head_;
	size_t tail_;
	size_t contents_size_;
};

template<typename T>
inline circular_buffer<T>::circular_buffer(size_t size)
	:array_(new T[size]),array_size_(size),head_(0),tail_(0),contents_size_(0){}

template<typename T>
inline circular_buffer<T>::circular_buffer(const circular_buffer<T>& rhs)
	: array_(new T[rhs.array_size_]), array_size_(rhs.array_size_), head_(rhs.head_), tail_(rhs.tail_), contents_size_(rhs.contents_size_) {
	std::copy_n(rhs.array_, rhs.array_size_, array_);
}

template<typename T>
inline circular_buffer<T>::~circular_buffer() {
	delete[] array_;
}

template<typename T>
inline void circular_buffer<T>::pop_front() {
	if (++head_ == array_size_)
		head_ = 0;
	--contents_size_;
}

template<typename T>
inline void circular_buffer<T>::push_back(const T& new_value) {
	assert(tail_ >= 0 && tail_ < array_size_);
	array_[tail_] = new_value;
	if (++tail_ == array_size_)
		tail_ = 0;
	if (contents_size_ < array_size_)
		++contents_size_;
	else
		++head_;
	if (head_ == array_size_)
		head_ = 0;
}

template<typename T>
inline void circular_buffer<T>::swap(const circular_buffer<T>& rhs) {
	std::swap(array_, rhs.array_);
	std::swap(array_size_, rhs.array_size_);
	std::swap(head_, rhs.head_);
	std::swap(tail_, rhs.tail_);
	std::swap(contents_size_, rhs.contents_size_);
}


