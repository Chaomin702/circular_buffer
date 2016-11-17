#pragma once
#include <allocators>
#include <assert.h>
#include <memory>
#include <iterator>
template <typename T, typename Alloc = std::allocator<T>>
class circular_buffer {
public:
	typedef circular_buffer<T, Alloc> self_type;
	typedef Alloc allocator_type;
	typedef typename Alloc::value_type value_type;
	typedef typename Alloc::pointer pointer;
	typedef typename Alloc::const_pointer const_pointer;
	typedef typename Alloc::reference reference;
	typedef typename Alloc::const_reference const_reference;
	typedef typename Alloc::size_type size_type;
	typedef typename Alloc::difference_type difference_type;

	template <typename E>
	class iterator_ : public std::iterator<std::random_access_iterator_tag, E> {
	public:
		typedef E elem_type;
		typedef iterator_<elem_type> self_type;
		typedef circular_buffer<T, Alloc> circular_buffer_type;
		iterator_(const circular_buffer_type *  buf,  const pointer  pos)
			:buf_(buf),pos_(pos){ }
		iterator_(const self_type& rhs)
			:buf_(rhs.buf_),pos_(rhs.pos_){ }
		self_type& operator=(self_type rhs) {
			swap(rhs);
			return *this;
		}
		void swap(const self_type& rhs) {
			std::swap(buf_, rhs.buf_);
			std::swap(pos_, rhs.pos_);
		}
		elem_type& operator*() {
			return *pos_;
		}
		elem_type* operator->() {
			return &(this->operator*());
		}
		self_type& operator++() {
			if (++pos_ == buf_->array_end_)
				pos_ = buf_->array_base_;
			if (pos_ == buf_->tail_)
				pos_ = 0;
			return *this;
		}
		self_type operator++(int) {
			self_type temp(*this);
			++(*this);
			return temp;
		}
		bool operator==(const self_type& rhs) {
			return buf_ == rhs.buf_ && pos_ == rhs.pos_;
		}
		bool operator!=(const self_type& rhs) {
			return !operator==(rhs);
		}
	private:
		const circular_buffer_type* buf_;
		pointer pos_;
	};
	explicit circular_buffer(const allocator_type& alloc = allocator_type()):
		: array_(0), 
		array_size_(0), 
		head_(0), 
		tail_(0), 
		contents_size_(0), 
		allocator_(alloc) {}
	explicit circular_buffer(size_type capacity = 1, const allocator_type& alloc = allocator_type())
		:array_base_(allocator_.allocate(capacity)), 
		array_end_(array_base_+capacity), 
		array_size_(capacity), 
		head_(array_base_), 
		tail_(array_base_), 
		contents_size_(0), 
		allocator_(alloc) {}

	//circular_buffer(const circular_buffer<T>& rhs)
	//	: array_(new T[rhs.array_size_]), array_size_(rhs.array_size_), head_(rhs.head_), tail_(rhs.tail_), contents_size_(rhs.contents_size_) {
	//	std::copy_n(rhs.array_, rhs.array_size_, array_);
	//}
	typedef iterator_<value_type> iterator;

	iterator begin() {
		return iterator(this, head_);
	}
	iterator end() {
		return iterator(this, 0);
	}
	circular_buffer<T>& operator = (circular_buffer<T> rhs) {
		swap(rhs);
		return *this;
	}
	~circular_buffer() {
		delete[] array_base_;
	}
	size_type size()const { return contents_size_; }
	bool empty()const { return contents_size_ == 0; }
	void pop_front() {
		if (++head_ == array_end_)
			head_ = array_base_;
		--contents_size_;
	}
	void push_back(const T& new_value) {
		assert(tail_ >= array_base_ && tail_ <= array_end_);
		*tail_ = new_value;
		if (++tail_ == array_end_)
			tail_ = array_base_;
		if (contents_size_ < array_size_)
			++contents_size_;
		else
			++head_;
		if (head_ == array_end_)
			head_ = array_base_;
	}
	void swap(const circular_buffer<T>& rhs) {
		std::swap(array_base_, rhs.array_base_);
		std::swap(array_size_, rhs.array_size_);
		std::swap(head_, rhs.head_);
		std::swap(tail_, rhs.tail_);
		std::swap(contents_size_, rhs.contents_size_);
	}
	const T& top()const {
		return *head_;
	}
	const T& operator[](size_type index)const {
		assert(index < size());
		return array_base_[(head_ - array_base_ + index) % array_size_];
	}
	T& operator[](size_type index) {
		return const_cast<T&>(const_cast<const circular_buffer<T>*>(this)->operator[](index));
	}
	void clear() {
		head_ = tail_ = array_base_;
		contents_size_ = 0;
	}
private:
	pointer array_base_;
	pointer array_end_;
	size_type array_size_;
	pointer head_;
	pointer tail_;
	size_type contents_size_;
	allocator_type allocator_;
};

