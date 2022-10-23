#pragma once

#include <cassert>
#include <initializer_list>
#include <algorithm>
#include <stdexcept>
#include <utility>
#include <iterator>

#include "array_ptr.h"

template< class ForwardIt, class Type >
void FillByMove(ForwardIt first, ForwardIt last, Type&&)
{
    for (; first != last; ++first) {
        *first = std::move(Type());
    }
}

class ReserveProxyObj {
public:
    ReserveProxyObj(size_t capacity_to_reserve)
        :size_(capacity_to_reserve)
    {
    }

    size_t size_ = 0;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size)
        :size_(size), capacity_(size), items_(size)
    {
        std::fill(&items_[0], &items_[size], Type());
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value)
        :size_(size), capacity_(size), items_(size)
    {
        std::fill(&items_[0], &items_[size], value);
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init)
        :size_(init.size()), capacity_(init.size()), items_(init.size())
    {
        uint64_t i = 0;
        for (auto const& value : init) {
            items_[i] = value;
            ++i;
        }
    }

    SimpleVector(const SimpleVector& other)
        :size_(other.size_), capacity_(other.size_)
    {    
        ArrayPtr<Type> temp(other.size_);
        std::copy(other.begin(), other.end(), &temp[0]);
        items_.swap(temp);
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        assert(this->items_.Get() != rhs.items_.Get());

        ArrayPtr<Type> temp(rhs.size_);
        std::copy(rhs.begin(), rhs.end(), &temp[0]);
        items_.swap(temp);
        size_ = rhs.size_;
        capacity_ = rhs.size_;
        return *this;
    }

    SimpleVector(ReserveProxyObj const& obj)
        :capacity_(obj.size_), items_(obj.size_)
    {
    }

    SimpleVector(SimpleVector&& other) noexcept
        :size_(other.size_), capacity_(other.capacity_)
    {
        this->items_ = std::move(other.items_);
        other.size_ = 0;
        other.capacity_ = 0;

    };
    SimpleVector& operator=(SimpleVector&& other) noexcept {
        this->items_ = std::move(other.items_);

        this->size_ = std::move(other.size_);
        other.size_ = 0;
        this->capacity_ = std::move(other.capacity_);
        other.capacity_ = 0;

        return *this;
    }

    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return !size_;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        assert(index < size_);
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        assert(index < size_);
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if (index >= size_)
            throw std::out_of_range("Element's index is incorrect (bigger than size)");
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if (index >= size_)
            throw std::out_of_range("Element's index is incorrect (bigger than size)");
        return items_[index];
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        this->Resize(0);
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        if (new_size == size_)
            return;
        if (new_size < size_)
            size_ = new_size;
        if (new_size > size_ && new_size <= capacity_) {
            Type* test = &items_[6];
            FillByMove(&items_[size_], &items_[new_size], Type());
            size_ = new_size;
        }
        if (new_size > capacity_) {
            Reserve(std::max(capacity_ * 2, new_size));
            size_ = new_size;
        }
    }

    void PopBack() noexcept {
        if (size_ == 0u)
            return;
        --size_;
    }

    Iterator Erase(ConstIterator pos) {
        assert(pos >= begin() && pos < end());
        auto it = &items_[std::distance(cbegin(), pos)];
        auto next_it = std::next(it);
        for (auto temp_it = it; next_it != cend(); ++temp_it, ++next_it) {
            std::exchange(*temp_it, std::move(*next_it));
        }
        --size_;
        return it;
    }

    void PushBack(const Type& value) {
        if (size_ == capacity_)
            Reserve(capacity_ * 2);
        items_[size_] = value;
        ++size_;
    }

    void PushBack(Type&& value) {
        if (size_ == capacity_)
            Reserve(capacity_ * 2);
        std::exchange(items_[size_], std::move(value));
        ++size_;
    }

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type& value) {
        assert(pos >= begin() && pos < end());
        auto shift = std::distance(cbegin(), pos);
        if (size_ == capacity_)
            Reserve(capacity_ * 2);
        auto pos2{ const_cast<Iterator>(std::next(cbegin(), shift)) };
        
        std::copy_backward(pos2, end(), std::next(end()));
        *pos2 = value;
        ++size_;
        return pos2;
    }

    Iterator Insert(ConstIterator pos, Type&& value) {
        assert(pos >= begin() && pos < end());
        auto shift = std::distance(cbegin(), pos);
        if (size_ == capacity_)
            Reserve(capacity_ * 2);
        auto pos2{ const_cast<Iterator>(std::next(cbegin(), shift)) };

        std::move_backward(pos2, end(), std::next(end()));
        *pos2 = std::move(value);
        ++size_;
        return pos2;
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return &items_[0];
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return &items_[size_];
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return &items_[0];
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return &items_[size_];
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return &items_[0];
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return &items_[size_];
    }

    void swap(SimpleVector& rhs) noexcept {
        this->items_.swap(rhs.items_);
        std::swap(this->size_, rhs.size_);
        std::swap(this->capacity_, rhs.capacity_);
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity == 0u)
            new_capacity = 1;
        if (new_capacity <= capacity_)
            return;
        ArrayPtr<Type> temp(new_capacity);
        std::move(begin(), end(), &temp[0]);
        FillByMove(&temp[size_], &temp[new_capacity], Type());
        items_.swap(temp);
        capacity_ = new_capacity;
    }

    private: 
        size_t size_ = 0;
        size_t capacity_ = 0;
        ArrayPtr<Type> items_;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    if (lhs.GetSize() != rhs.GetSize())
        return false;
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    if (lhs == rhs)
        return false;
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    if (lhs == rhs)
        return true;
    return lhs < rhs;
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    if (lhs == rhs)
        return false;
    return !(lhs < rhs);
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    if (lhs == rhs)
        return true;
    return lhs > rhs;
}