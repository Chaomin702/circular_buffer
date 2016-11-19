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
	explicit circular_buffer(const allocator_type& alloc = allocator_type())
		: array_base_(0),
		array_size_(0),
		head_(0),
		tail_(0),
		contents_size_(0),
		allocator_(alloc) {}
	explicit circular_buffer(size_type capacity = 1, const allocator_type& alloc = allocator_type())
		:array_base_(allocator_.allocate(capacity)),
		array_end_(array_base_ + capacity),
		array_size_(capacity),
		head_(array_base_),
		tail_(array_base_),
		contents_size_(0),
		allocator_(alloc) {
	}

	circular_buffer(const self_type& rhs)
		:array_size_(rhs.array_size_),
		contents_size_(rhs.contents_size_),
		allocator_(rhs.allocator_) {
		array_base_ = allocator_.allocate(array_size_);
		array_end_ = array_base_ + array_size_;
		tail_ = std::uninitialized_copy(rhs.begin(), rhs.end(), array_base_);
		if (tail_ == array_end_)
			tail_ = array_base_;
		head_ = array_base_;
	}
	self_type& operator = (self_type rhs) {
		swap(rhs);
		return *this;
	}
	void swap(self_type& rhs) noexcept {
		using std::swap;
		swap(array_base_, rhs.array_base_);
		swap(array_end_, rhs.array_end_);
		swap(array_size_, rhs.array_size_);
		swap(head_, rhs.head_);
		swap(tail_, rhs.tail_);
		swap(contents_size_, rhs.contents_size_);
	}
	~circular_buffer() {
		if (array_base_) {
			for (auto it = begin(); it != end(); ++it) {
				allocator_.destroy(&*it);
			}
			allocator_.deallocate(array_base_, array_size_);
		}
	}
	template <typename E>
	class iterator_ : public std::iterator<std::random_access_iterator_tag, E> {
	public:
		typedef E elem_type;
		typedef iterator_<elem_type> self_type;
		typedef circular_buffer<T, Alloc> circular_buffer_type;
		iterator_(const circular_buffer_type *  buf,  pointer  pos)
			:buf_(buf),pos_(pos){ }
		iterator_(const self_type& rhs)
			:buf_(rhs.buf_),pos_(rhs.pos_){ }
		self_type& operator=(self_type rhs) {
			swap(rhs);
			return *this;
		}
		void swap(self_type& rhs) {
			std::swap(buf_, rhs.buf_);
			std::swap(pos_, rhs.pos_);
		}
		elem_type& operator*() const{
			return *pos_;
		}
		elem_type* operator->() const{
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
		bool operator==(const self_type& rhs)const {
			return buf_ == rhs.buf_ && pos_ == rhs.pos_;
		}
		bool operator!=(const self_type& rhs)const {
			return !operator==(rhs);
		}

		difference_type operator-(const self_type& rhs)const {
			return linearize_pointer(*this) - linearize_pointer(rhs);
		}
		self_type& operator+=(const difference_type n) {
			if (n > 0) {
				pos_ = buf_->add(pos_, n);
				if (pos_ == buf_->tail_)
					pos_ = 0;
			}else if(n < 0) {
				*this -= -n;
			}
			return *this;
		}
		self_type& operator-=(const difference_type n) {
			if (n > 0) {
				pos_ = buf_->sub(pos_ == 0 ? buf_->tail_ : pos_, n);
			}else if(n <0){
				*this += -n;
			}
			return *this;
		}
		self_type operator+(const difference_type n)const {
			return self_type(*this) += n;
		}
		self_type operator-(const difference_type n)const {
			return self_type(*this) -= n;
		}
		bool operator<(const self_type&rhs)const {
			return linearize_pointer(*this) < linearize_pointer(rhs);
		}
		bool operator>(const self_type&rhs)const {
			return rhs < *this;
		}
		pointer linearize_pointer(const self_type& it)const {		//for circular memory, we need linearize it
			return (it.pos_ == 0) ? buf_->array_end_ :
				(it.pos_ < buf_->head_ ? it.pos_ + (buf_->array_end_ - buf_->head_) 
					: buf_->array_base_ + (it.pos_ - buf_->head_));
		}
	private:
		const circular_buffer_type*  buf_;
		pointer  pos_;
	};
	typedef iterator_<value_type> iterator;
	typedef iterator_<const value_type> const_iterator;
	iterator begin() {
		return iterator(this, empty() ? 0 : head_);
	}
	iterator end() {
		return iterator(this, 0);
	}
	const_iterator begin()const {
		return const_iterator(this, empty() ? 0 : head_);
	}
	const_iterator end()const {
		return const_iterator(this, 0);
	}
	size_type size()const { return contents_size_; }
	size_type capacity()const { return array_size_; }
	bool empty()const { return size() == 0; }
	bool full()const { return size() == capacity(); }
	void pop_front() {
		allocator_.destroy(head_);
		if (++head_ == array_end_)
			head_ = array_base_;
		--contents_size_;
	}
	void push_back(const_reference new_value) {
		assert(tail_ >= array_base_ && tail_ <= array_end_);
		if (full())
			*tail_ = new_value;
		else
			allocator_.construct(tail_, new_value);
		if (++tail_ == array_end_)
			tail_ = array_base_;
		if (contents_size_ < array_size_)
			++contents_size_;
		else
			++head_;
		if (head_ == array_end_)
			head_ = array_base_;
	}
	const_reference top()const {
		return *head_;
	}
	const_reference operator[](size_type index)const {
		assert(index < size());
		return array_base_[(head_ - array_base_ + index) % array_size_];
	}
	reference operator[](size_type index) {
		return const_cast<T&>(const_cast<const circular_buffer<T>*>(this)->operator[](index));
	}
	reference front() {
		return *head_;
	}
	const_reference front()const {
		return *head_;
	}
	reference back() {
		return *((tail_ == array_base_ ? array_end_ : tail_) - 1);
	}
	const_reference back()const {
		return *((tail_ == array_base_ ? array_end_ : tail_) - 1);
	}
	void clear() {
		for (auto it = begin(); it != end(); ++it) {
			allocator_.destroy(&*it);
		}
		head_ = tail_ = array_base_;
		contents_size_ = 0;
	}
private:
	template <class Pointer>
	Pointer add(Pointer p, difference_type n)const {
		return p + (n < (array_end_ - p) ? n : n - capacity());
	}
	template <class Pointer>
	Pointer sub(Pointer p, difference_type n)const {
		return p - (n > (p - array_base_) ? n - capacity() : n);
	}
	pointer array_base_;	
	pointer array_end_;
	size_type array_size_;
	pointer head_;		//point to first element 
	pointer tail_;		//point to next element of last, which should be destroy
	size_type contents_size_;
	allocator_type allocator_;
};

