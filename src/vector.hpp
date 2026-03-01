#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <cstddef>
#include <new>

namespace sjtu
{
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
template<typename T>
class vector
{
private:
	T *data_;
	size_t size_;
	size_t capacity_;

	static T *allocate_raw(size_t n)
	{
		if (n == 0) return nullptr;
		return static_cast<T *>(::operator new(sizeof(T) * n));
	}
	static void deallocate_raw(T *ptr)
	{
		::operator delete(ptr);
	}
	static void destroy_range(T *ptr, size_t n)
	{
		for (size_t i = 0; i < n; ++i) {
			ptr[i].~T();
		}
	}
	void ensure_capacity(size_t need)
	{
		if (capacity_ >= need) return;
		size_t new_cap = capacity_ == 0 ? 1 : capacity_;
		while (new_cap < need) new_cap <<= 1;
		T *new_data = allocate_raw(new_cap);
		size_t constructed = 0;
		try {
			for (; constructed < size_; ++constructed) {
				new (new_data + constructed) T(data_[constructed]);
			}
		} catch (...) {
			destroy_range(new_data, constructed);
			deallocate_raw(new_data);
			throw;
		}
		destroy_range(data_, size_);
		deallocate_raw(data_);
		data_ = new_data;
		capacity_ = new_cap;
	}

	void check_index(const size_t &pos) const
	{
		if (pos >= size_) throw index_out_of_bound();
	}

public:
	class const_iterator;
	class iterator
	{
	public:
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using iterator_category = void;

	private:
		vector<T> *owner_;
		size_t index_;

	public:
		iterator(vector<T> *owner = nullptr, size_t index = 0) : owner_(owner), index_(index) {}

		iterator operator+(const int &n) const
		{
			return iterator(owner_, static_cast<size_t>(static_cast<long long>(index_) + n));
		}
		iterator operator-(const int &n) const
		{
			return iterator(owner_, static_cast<size_t>(static_cast<long long>(index_) - n));
		}
		int operator-(const iterator &rhs) const
		{
			if (owner_ != rhs.owner_) throw invalid_iterator();
			return static_cast<int>(static_cast<long long>(index_) - static_cast<long long>(rhs.index_));
		}
		iterator& operator+=(const int &n)
		{
			index_ = static_cast<size_t>(static_cast<long long>(index_) + n);
			return *this;
		}
		iterator& operator-=(const int &n)
		{
			index_ = static_cast<size_t>(static_cast<long long>(index_) - n);
			return *this;
		}
		iterator operator++(int)
		{
			iterator tmp(*this);
			++(*this);
			return tmp;
		}
		iterator& operator++()
		{
			if (owner_ == nullptr || index_ >= owner_->size_) throw invalid_iterator();
			++index_;
			return *this;
		}
		iterator operator--(int)
		{
			iterator tmp(*this);
			--(*this);
			return tmp;
		}
		iterator& operator--()
		{
			if (owner_ == nullptr || index_ == 0 || index_ > owner_->size_) throw invalid_iterator();
			--index_;
			return *this;
		}
		T& operator*() const
		{
			if (owner_ == nullptr || index_ >= owner_->size_) throw invalid_iterator();
			return owner_->data_[index_];
		}
		bool operator==(const iterator &rhs) const
		{
			return owner_ == rhs.owner_ && index_ == rhs.index_;
		}
		bool operator==(const const_iterator &rhs) const
		{
			return owner_ == rhs.owner_ && index_ == rhs.index_;
		}
		bool operator!=(const iterator &rhs) const
		{
			return !(*this == rhs);
		}
		bool operator!=(const const_iterator &rhs) const
		{
			return !(*this == rhs);
		}

		friend class vector<T>;
		friend class const_iterator;
	};

	class const_iterator
	{
	public:
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = const T*;
		using reference = const T&;
		using iterator_category = void;

	private:
		const vector<T> *owner_;
		size_t index_;

	public:
		const_iterator(const vector<T> *owner = nullptr, size_t index = 0) : owner_(owner), index_(index) {}
		const_iterator(const iterator &other) : owner_(other.owner_), index_(other.index_) {}

		const_iterator operator+(const int &n) const
		{
			return const_iterator(owner_, static_cast<size_t>(static_cast<long long>(index_) + n));
		}
		const_iterator operator-(const int &n) const
		{
			return const_iterator(owner_, static_cast<size_t>(static_cast<long long>(index_) - n));
		}
		int operator-(const const_iterator &rhs) const
		{
			if (owner_ != rhs.owner_) throw invalid_iterator();
			return static_cast<int>(static_cast<long long>(index_) - static_cast<long long>(rhs.index_));
		}
		int operator-(const iterator &rhs) const
		{
			if (owner_ != rhs.owner_) throw invalid_iterator();
			return static_cast<int>(static_cast<long long>(index_) - static_cast<long long>(rhs.index_));
		}
		const_iterator& operator+=(const int &n)
		{
			index_ = static_cast<size_t>(static_cast<long long>(index_) + n);
			return *this;
		}
		const_iterator& operator-=(const int &n)
		{
			index_ = static_cast<size_t>(static_cast<long long>(index_) - n);
			return *this;
		}
		const_iterator operator++(int)
		{
			const_iterator tmp(*this);
			++(*this);
			return tmp;
		}
		const_iterator& operator++()
		{
			if (owner_ == nullptr || index_ >= owner_->size_) throw invalid_iterator();
			++index_;
			return *this;
		}
		const_iterator operator--(int)
		{
			const_iterator tmp(*this);
			--(*this);
			return tmp;
		}
		const_iterator& operator--()
		{
			if (owner_ == nullptr || index_ == 0 || index_ > owner_->size_) throw invalid_iterator();
			--index_;
			return *this;
		}
		const T& operator*() const
		{
			if (owner_ == nullptr || index_ >= owner_->size_) throw invalid_iterator();
			return owner_->data_[index_];
		}
		bool operator==(const iterator &rhs) const
		{
			return owner_ == rhs.owner_ && index_ == rhs.index_;
		}
		bool operator==(const const_iterator &rhs) const
		{
			return owner_ == rhs.owner_ && index_ == rhs.index_;
		}
		bool operator!=(const iterator &rhs) const
		{
			return !(*this == rhs);
		}
		bool operator!=(const const_iterator &rhs) const
		{
			return !(*this == rhs);
		}

		friend class vector<T>;
		friend class iterator;
	};

	vector() : data_(nullptr), size_(0), capacity_(0) {}

	vector(const vector &other) : data_(nullptr), size_(0), capacity_(0)
	{
		if (other.size_ == 0) return;
		data_ = allocate_raw(other.size_);
		capacity_ = other.size_;
		size_t constructed = 0;
		try {
			for (; constructed < other.size_; ++constructed) {
				new (data_ + constructed) T(other.data_[constructed]);
			}
			size_ = other.size_;
		} catch (...) {
			destroy_range(data_, constructed);
			deallocate_raw(data_);
			data_ = nullptr;
			capacity_ = 0;
			throw;
		}
	}

	~vector()
	{
		destroy_range(data_, size_);
		deallocate_raw(data_);
	}

	vector &operator=(const vector &other)
	{
		if (this == &other) return *this;
		vector tmp(other);
		T *old_data = data_;
		size_t old_size = size_;
		size_t old_capacity = capacity_;
		data_ = tmp.data_;
		size_ = tmp.size_;
		capacity_ = tmp.capacity_;
		tmp.data_ = old_data;
		tmp.size_ = old_size;
		tmp.capacity_ = old_capacity;
		return *this;
	}

	T & at(const size_t &pos)
	{
		check_index(pos);
		return data_[pos];
	}
	const T & at(const size_t &pos) const
	{
		check_index(pos);
		return data_[pos];
	}

	T & operator[](const size_t &pos)
	{
		check_index(pos);
		return data_[pos];
	}
	const T & operator[](const size_t &pos) const
	{
		check_index(pos);
		return data_[pos];
	}

	const T & front() const
	{
		if (size_ == 0) throw container_is_empty();
		return data_[0];
	}
	const T & back() const
	{
		if (size_ == 0) throw container_is_empty();
		return data_[size_ - 1];
	}

	iterator begin() { return iterator(this, 0); }
	const_iterator begin() const { return const_iterator(this, 0); }
	const_iterator cbegin() const { return const_iterator(this, 0); }

	iterator end() { return iterator(this, size_); }
	const_iterator end() const { return const_iterator(this, size_); }
	const_iterator cend() const { return const_iterator(this, size_); }

	bool empty() const { return size_ == 0; }
	size_t size() const { return size_; }

	void clear()
	{
		destroy_range(data_, size_);
		deallocate_raw(data_);
		data_ = nullptr;
		size_ = 0;
		capacity_ = 0;
	}

	iterator insert(iterator pos, const T &value)
	{
		if (pos.owner_ != this || pos.index_ > size_) throw invalid_iterator();
		return insert(pos.index_, value);
	}

	iterator insert(const size_t &ind, const T &value)
	{
		if (ind > size_) throw index_out_of_bound();
		ensure_capacity(size_ + 1);
		if (ind == size_) {
			new (data_ + size_) T(value);
			++size_;
			return iterator(this, ind);
		}

		new (data_ + size_) T(data_[size_ - 1]);
		for (size_t i = size_ - 1; i > ind; --i) {
			data_[i] = data_[i - 1];
		}
		data_[ind] = value;
		++size_;
		return iterator(this, ind);
	}

	iterator erase(iterator pos)
	{
		if (pos.owner_ != this || pos.index_ >= size_) throw invalid_iterator();
		return erase(pos.index_);
	}

	iterator erase(const size_t &ind)
	{
		if (ind >= size_) throw index_out_of_bound();
		for (size_t i = ind; i + 1 < size_; ++i) {
			data_[i] = data_[i + 1];
		}
		data_[size_ - 1].~T();
		--size_;
		return iterator(this, ind);
	}

	void push_back(const T &value)
	{
		ensure_capacity(size_ + 1);
		new (data_ + size_) T(value);
		++size_;
	}

	void pop_back()
	{
		if (size_ == 0) throw container_is_empty();
		data_[size_ - 1].~T();
		--size_;
	}
};


}

#endif
