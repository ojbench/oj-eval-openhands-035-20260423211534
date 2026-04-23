#ifndef SIMPLE_STRING_SIMPLESTRING_HPP
#define SIMPLE_STRING_SIMPLESTRING_HPP

#include <stdexcept>
#include <cstring>

class MyString {
private:
    union {
        char* heap_ptr;
        char small_buffer[16];
    };
    size_t len;
    size_t cap;
    
    bool is_sso() const {
        return cap <= 15;
    }
    
    char* data_ptr() {
        return is_sso() ? small_buffer : heap_ptr;
    }
    
    const char* data_ptr() const {
        return is_sso() ? small_buffer : heap_ptr;
    }

public:
    MyString() : len(0), cap(15) {
        small_buffer[0] = '\0';
    }

    MyString(const char* s) {
        if (s == nullptr) {
            len = 0;
            cap = 15;
            small_buffer[0] = '\0';
            return;
        }
        
        len = strlen(s);
        
        if (len <= 15) {
            cap = 15;
            memcpy(small_buffer, s, len + 1);
        } else {
            cap = len;
            heap_ptr = new char[cap + 1];
            memcpy(heap_ptr, s, len + 1);
        }
    }

    MyString(const MyString& other) : len(other.len), cap(other.cap) {
        if (other.is_sso()) {
            memcpy(small_buffer, other.small_buffer, len + 1);
        } else {
            heap_ptr = new char[cap + 1];
            memcpy(heap_ptr, other.heap_ptr, len + 1);
        }
    }

    MyString(MyString&& other) noexcept : len(other.len), cap(other.cap) {
        if (other.is_sso()) {
            memcpy(small_buffer, other.small_buffer, len + 1);
        } else {
            heap_ptr = other.heap_ptr;
            other.heap_ptr = nullptr;
        }
        other.len = 0;
        other.cap = 15;
        other.small_buffer[0] = '\0';
    }

    MyString& operator=(MyString&& other) noexcept {
        if (this != &other) {
            if (!is_sso()) {
                delete[] heap_ptr;
            }
            
            len = other.len;
            cap = other.cap;
            
            if (other.is_sso()) {
                memcpy(small_buffer, other.small_buffer, len + 1);
            } else {
                heap_ptr = other.heap_ptr;
                other.heap_ptr = nullptr;
            }
            
            other.len = 0;
            other.cap = 15;
            other.small_buffer[0] = '\0';
        }
        return *this;
    }

    MyString& operator=(const MyString& other) {
        if (this != &other) {
            if (!is_sso()) {
                delete[] heap_ptr;
            }
            
            len = other.len;
            cap = other.cap;
            
            if (other.is_sso()) {
                memcpy(small_buffer, other.small_buffer, len + 1);
            } else {
                heap_ptr = new char[cap + 1];
                memcpy(heap_ptr, other.heap_ptr, len + 1);
            }
        }
        return *this;
    }

    ~MyString() {
        if (!is_sso()) {
            delete[] heap_ptr;
        }
    }

    const char* c_str() const {
        return data_ptr();
    }

    size_t size() const {
        return len;
    }

    size_t capacity() const {
        return cap;
    }

    void reserve(size_t new_capacity) {
        if (new_capacity <= cap) {
            return;
        }
        
        if (new_capacity <= 15) {
            return;
        }
        
        bool was_sso = is_sso();
        char* new_ptr = new char[new_capacity + 1];
        memcpy(new_ptr, data_ptr(), len + 1);
        
        if (!was_sso) {
            delete[] heap_ptr;
        }
        
        heap_ptr = new_ptr;
        cap = new_capacity;
    }

    void resize(size_t new_size) {
        if (new_size > cap) {
            // Grow capacity similar to append behavior
            size_t new_cap = cap * 2;
            if (new_cap < new_size) {
                new_cap = new_size;
            }
            reserve(new_cap);
        }
        
        if (new_size > len) {
            memset(data_ptr() + len, '\0', new_size - len);
        }
        
        len = new_size;
        data_ptr()[len] = '\0';
    }

    char& operator[](size_t index) {
        return data_ptr()[index];
    }
    
    const char& operator[](size_t index) const {
        return data_ptr()[index];
    }

    MyString operator+(const MyString& rhs) const {
        MyString result;
        size_t new_len = len + rhs.len;
        
        if (new_len > 15) {
            result.cap = new_len;
            result.heap_ptr = new char[new_len + 1];
        } else {
            result.cap = 15;
        }
        
        result.len = new_len;
        memcpy(result.data_ptr(), data_ptr(), len);
        memcpy(result.data_ptr() + len, rhs.data_ptr(), rhs.len + 1);
        
        return result;
    }

    void append(const char* str) {
        if (str == nullptr) {
            return;
        }
        
        size_t str_len = strlen(str);
        size_t new_len = len + str_len;
        
        if (new_len > cap) {
            size_t new_cap = cap * 2;
            if (new_cap < new_len) {
                new_cap = new_len;
            }
            reserve(new_cap);
        }
        
        memcpy(data_ptr() + len, str, str_len + 1);
        len = new_len;
    }

    const char& at(size_t pos) const {
        if (pos >= len) {
            throw std::out_of_range("Index out of range");
        }
        return data_ptr()[pos];
    }
    
    char& at(size_t pos) {
        if (pos >= len) {
            throw std::out_of_range("Index out of range");
        }
        return data_ptr()[pos];
    }

    class const_iterator;

    class iterator {
    private:
        char* ptr;
        
        friend class MyString;
        friend class const_iterator;
        
        iterator(char* p) : ptr(p) {}
        
    public:
        iterator& operator++() {
            ++ptr;
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            ++ptr;
            return tmp;
        }

        iterator& operator--() {
            --ptr;
            return *this;
        }

        iterator operator--(int) {
            iterator tmp = *this;
            --ptr;
            return tmp;
        }

        char& operator*() const {
            return *ptr;
        }

        bool operator==(const iterator& other) const {
            return ptr == other.ptr;
        }

        bool operator!=(const iterator& other) const {
            return ptr != other.ptr;
        }

        bool operator==(const const_iterator& other) const {
            return ptr == other.ptr;
        }

        bool operator!=(const const_iterator& other) const {
            return ptr != other.ptr;
        }
    };

    class const_iterator {
    private:
        const char* ptr;
        
        friend class MyString;
        friend class iterator;
        
        const_iterator(const char* p) : ptr(p) {}
        
    public:
        const_iterator& operator++() {
            ++ptr;
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator tmp = *this;
            ++ptr;
            return tmp;
        }

        const_iterator& operator--() {
            --ptr;
            return *this;
        }

        const_iterator operator--(int) {
            const_iterator tmp = *this;
            --ptr;
            return tmp;
        }

        const char& operator*() const {
            return *ptr;
        }

        bool operator==(const const_iterator& other) const {
            return ptr == other.ptr;
        }

        bool operator!=(const const_iterator& other) const {
            return ptr != other.ptr;
        }

        bool operator==(const iterator& other) const {
            return ptr == other.ptr;
        }

        bool operator!=(const iterator& other) const {
            return ptr != other.ptr;
        }
    };

public:
    iterator begin() {
        return iterator(data_ptr());
    }

    iterator end() {
        return iterator(data_ptr() + len);
    }

    const_iterator cbegin() const {
        return const_iterator(data_ptr());
    }

    const_iterator cend() const {
        return const_iterator(data_ptr() + len);
    }
};

#endif
