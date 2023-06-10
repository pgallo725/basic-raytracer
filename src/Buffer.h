#pragma once


#ifdef _MSC_VER
 #define aligned_alloc(size, align) _aligned_malloc(size, align)
 #define aligned_free(pointer)      _aligned_free(pointer)
#else
 #define aligned_alloc(size, align) std::aligned_alloc(align, size)
 #define aligned_free(pointer)      std::free(pointer)
#endif


template<typename T, size_t Alignment>
class Buffer
{
	static_assert(std::is_default_constructible_v<T>,
		"Type <T> must be default-constructible");

public:

	Buffer() = default;
	Buffer(const T* data, size_t count)
	{
		if (Allocate(count))
			for (size_t i = 0; i < count; i++)
				this->data[i] = data[i];
	}

	Buffer(const Buffer&) = delete;
	Buffer& operator=(const Buffer&) = delete;

	Buffer(Buffer&& other) noexcept
	{
		data = other.data;
		count = other.count;
		other.data = nullptr;
		other.count = 0;
	}
	Buffer& operator=(Buffer&& other) noexcept
	{
		if (this != &other)
		{
			data = other.data;
			count = other.count;
			other.data = nullptr;
			other.count = 0;
		}
		return *this;
	}

	~Buffer() { Free();	}


	bool Allocate(size_t capacity)
	{
		if (count == capacity)
			return true;

		Free();

		if (capacity == 0)
			return true;

		this->data = static_cast<T*>(aligned_alloc(capacity * sizeof(T), Alignment));
		this->count = (data != nullptr ? capacity : 0);

		if (data != nullptr)
		{
			// Initialize memory
			for (size_t i = 0; i < capacity; i++)
				new(&data[i]) T();
			return true;
		}
		return false;
	}

	void Free()
	{
		if (data)
			aligned_free(data);

		this->data = nullptr;
		this->count = 0;
	}

	template<size_t SourceAlignment>
	bool Copy(const Buffer<T, SourceAlignment>& other)
	{
		if (Allocate(other.count))
		{
			for (size_t i = 0; i < other.count; i++)
				this->data[i] = other.data[i];
			return true;
		}
		return false;
	}


	const T * GetDataPtr() const { return data; }
	      T * GetDataPtr()       { return data; }

	size_t GetCount()     const { return count; }
	size_t GetSizeBytes() const { return count * sizeof(T); }

	const T & operator[](size_t i) const { return data[i]; }
	      T & operator[](size_t i)       { return data[i]; }


private:

	T *    data = nullptr;
	size_t count = 0;
};