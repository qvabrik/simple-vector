#pragma once
#include <cassert>
#include <stdexcept>
#include <iostream>
#include <numeric>

#include "simple_vector.h"
#include "array_ptr.h"

// � �������, ����������� �� �������������� inline, ����� ���� ���������
// ���������� ����������� � ������ �������� ����������.
// ������ inline �������� �������, ��� ���� ��������� � ������������ �����,
// ����� ��� ����������� ����� ����� �� ������ ������ ���������� �� ��������� ������ ����������
inline void Test1() {
    // ������������� ������������� �� ���������
    {
        SimpleVector<int> v;
        assert(v.GetSize() == 0u);
        assert(v.IsEmpty());
        assert(v.GetCapacity() == 0u);
    }

    // ������������� ������� ���������� �������
    {
        SimpleVector<int> v(5);
        assert(v.GetSize() == 5u);
        assert(v.GetCapacity() == 5u);
        assert(!v.IsEmpty());
        for (size_t i = 0; i < v.GetSize(); ++i) {
            assert(v[i] == 0);
        }
    }

    // ������������� �������, ������������ �������� ���������
    {
        SimpleVector<int> v(3, 42);
        assert(v.GetSize() == 3);
        assert(v.GetCapacity() == 3);
        for (size_t i = 0; i < v.GetSize(); ++i) {
            assert(v[i] == 42);
        }
    }

    // ������������� ������� ��� ������ initializer_list
    {
        SimpleVector<int> v{ 1, 2, 3 };
        assert(v.GetSize() == 3);
        assert(v.GetCapacity() == 3);
        assert(v[2] == 3);
    }

    // ������ � ��������� ��� ������ At
    {
        SimpleVector<int> v(3);
        assert(&v.At(2) == &v[2]);
        try {
            v.At(3);
            assert(false);  // ��������� ������������ ����������
        }
        catch (const std::out_of_range&) {
        }
        catch (...) {
            assert(false);  // �� ��������� ����������, �������� �� out_of_range
        }
    }

    // ������� �������
    {
        SimpleVector<int> v(10);
        const size_t old_capacity = v.GetCapacity();
        v.Clear();
        assert(v.GetSize() == 0);
        assert(v.GetCapacity() == old_capacity);
    }

    // ��������� �������
    {
        SimpleVector<int> v(3);
        v[2] = 17;
        v.Resize(7);
        assert(v.GetSize() == 7);
        assert(v.GetCapacity() >= v.GetSize());
        assert(v[2] == 17);
        assert(v[3] == 0);
    }
    {
        SimpleVector<int> v(3);
        v[0] = 42;
        v[1] = 55;
        const size_t old_capacity = v.GetCapacity();
        v.Resize(2);
        assert(v.GetSize() == 2);
        assert(v.GetCapacity() == old_capacity);
        assert(v[0] == 42);
        assert(v[1] == 55);
    }
    {
        const size_t old_size = 3;
        SimpleVector<int> v(3);
        v.Resize(old_size + 5);
        v[3] = 42;
        v.Resize(old_size);
        v.Resize(old_size + 2);
        assert(v[3] == 0);
    }

    // ������������ �� SimpleVector
    {
        // ������ ������
        {
            SimpleVector<int> v;
            assert(v.begin() == nullptr);
            assert(v.end() == nullptr);
        }

        // �������� ������
        {
            SimpleVector<int> v(10, 42);
            assert(v.begin());
            assert(*v.begin() == 42);
            assert(v.end() == v.begin() + v.GetSize());
        }
    }
}

inline void Test2() {
    // PushBack
    {
        SimpleVector<int> v(1);
        v.PushBack(42);
        assert(v.GetSize() == 2);
        assert(v.GetCapacity() >= v.GetSize());
        assert(v[0] == 0);
        assert(v[1] == 42);
    }

    // ���� ������� �����, PushBack �� ����������� Capacity
    {
        SimpleVector<int> v(2);
        v.Resize(1);
        const size_t old_capacity = v.GetCapacity();
        v.PushBack(123);
        assert(v.GetSize() == 2);
        assert(v.GetCapacity() == old_capacity);
    }

    // PopBack
    {
        SimpleVector<int> v{ 0, 1, 2, 3 };
        const size_t old_capacity = v.GetCapacity();
        const auto old_begin = v.begin();
        v.PopBack();
        assert(v.GetCapacity() == old_capacity);
        assert(v.begin() == old_begin);
        assert((v == SimpleVector<int>{0, 1, 2}));
    }

    // ����������� �����������
    {
        SimpleVector<int> numbers{ 1, 2 };
        auto numbers_copy(numbers);
        assert(&numbers_copy[0] != &numbers[0]);
        assert(numbers_copy.GetSize() == numbers.GetSize());
        for (size_t i = 0; i < numbers.GetSize(); ++i) {
            assert(numbers_copy[i] == numbers[i]);
            assert(&numbers_copy[i] != &numbers[i]);
        }
    }

    // ���������
    {
        assert((SimpleVector<int>{ 1, 2, 3 } == SimpleVector<int>{ 1, 2, 3 }));
        assert((SimpleVector<int>{ 1, 2, 3 } != SimpleVector<int>{ 1, 2, 2 }));

        assert((SimpleVector<int>{ 1, 2, 3 } < SimpleVector<int>{ 1, 2, 3, 1 }));
        assert((SimpleVector<int>{ 1, 2, 3 } > SimpleVector<int>{ 1, 2, 2, 1 }));

        assert((SimpleVector<int>{ 1, 2, 3 } >= SimpleVector<int>{ 1, 2, 3 }));
        assert((SimpleVector<int>{ 1, 2, 4 } >= SimpleVector<int>{ 1, 2, 3 }));
        assert((SimpleVector<int>{ 1, 2, 3 } <= SimpleVector<int>{ 1, 2, 3 }));
        assert((SimpleVector<int>{ 1, 2, 3 } <= SimpleVector<int>{ 1, 2, 4 }));
    }

    //// ����� �������� ��������
    {
        SimpleVector<int> v1{ 42, 666 };
        SimpleVector<int> v2;
        v2.PushBack(0);
        v2.PushBack(1);
        v2.PushBack(2);
        const int* const begin1 = &v1[0];
        const int* const begin2 = &v2[0];

        const size_t capacity1 = v1.GetCapacity();
        const size_t capacity2 = v2.GetCapacity();

        const size_t size1 = v1.GetSize();
        const size_t size2 = v2.GetSize();

        static_assert(noexcept(v1.swap(v2)));
        v1.swap(v2);
        assert(&v2[0] == begin1);
        assert(&v1[0] == begin2);
        assert(v1.GetSize() == size2);
        assert(v2.GetSize() == size1);
        assert(v1.GetCapacity() == capacity2);
        assert(v2.GetCapacity() == capacity1);
    }

    // ������������
    {
        SimpleVector<int> src_vector{ 1, 2, 3, 4 };
        SimpleVector<int> dst_vector{ 1, 2, 3, 4, 5, 6 };
        dst_vector = src_vector;
        assert(dst_vector == src_vector);
    }

    // ������� ���������
    {
        SimpleVector<int> v{ 1, 2, 3, 4 };
        v.Insert(v.begin() + 2, 42);
        assert((v == SimpleVector<int>{1, 2, 42, 3, 4}));
    }

    // �������� ���������
    {
        SimpleVector<int> v{ 1, 2, 3, 4 };
        //int* test0 = &v[0];
        //int* test1 = &v[1];
        //int* test2 = &v[2];
        //int* test3 = &v[3];
        v.Erase(v.cbegin() + 2);
        assert((v == SimpleVector<int>{1, 2, 4}));
    }
}

void TestReserveConstructor() {
    using namespace std;
    cout << "TestReserveConstructor"s << endl;
    {
        SimpleVector<int> v(Reserve(5));
        assert(v.GetCapacity() == 5);
        assert(v.IsEmpty());
    }
    {
        SimpleVector<int> v(Reserve(0));
        assert(v.GetCapacity() == 0);
    }
    cout << "Done!"s << endl;
}

void TestReserveMethod() {
    using namespace std;
    cout << "TestReserveMethod"s << endl;
    SimpleVector<int> v;
    // ������������� 5 ���� � �������
    v.Reserve(5);
    assert(v.GetCapacity() == 5);
    assert(v.IsEmpty());

    // ���������� ��������� capacity �� 1
    v.Reserve(1);
    // capacity ������ �������� �������
    assert(v.GetCapacity() == 5);
    // �������� 10 ��������� � ������
    for (int i = 0; i < 10; ++i) {
        v.PushBack(i);
    }
    assert(v.GetSize() == 10);
    // �������� capacity �� 100
    v.Reserve(100);
    // ��������, ��� ������ �� ���������
    assert(v.GetSize() == 10);
    assert(v.GetCapacity() == 100);
    // ��������, ��� �������� �� �����
    for (int i = 0; i < 10; ++i) {
        assert(v[i] == i);
    }
    cout << "Done!"s << endl;
}

class X {
public:
    X()
        : X(5) {
    }

    X(size_t num)
        : x_(num) {
    }

    X(const X& other) = delete;

    X& operator=(const X& other) = delete;

    X(X&& other) noexcept {
        x_ = std::exchange(other.x_, 0);
    }

    X& operator=(X&& other) noexcept {
        x_ = std::exchange(other.x_, 0);
        return *this;
    }

    size_t GetX() const {
        return x_;
    }

private:
    size_t x_;
};

SimpleVector<int> GenerateVector(size_t size) {
    SimpleVector<int> v(size);
    std::iota(v.begin(), v.end(), 1);
    return v;
}

void TestMoveArrayPtr() {
    {
        int* test = new int[5]{ 1, 2, 3, 4, 5 };
        ArrayPtr<int> ptr_from(test);
        assert(&ptr_from[2] == &test[2]);
        
        ArrayPtr<int> ptr_to(std::move(ptr_from));
        assert(&ptr_to[2] == &test[2]);
        assert(ptr_from.Get() == nullptr);
    }
    {
        int* test = new int[5]{ 1, 2, 3, 4, 5 };
        ArrayPtr<int> ptr_from(test);

        ArrayPtr<int> ptr_to = std::move(ptr_from);
        assert(&ptr_to[2] == &test[2]);
        assert(ptr_from.Get() == nullptr);

        ptr_to = std::move(ptr_to);
        assert(&ptr_to[2] == &test[2]);
    }
}

void TestTemporaryObjConstructor() {
    const size_t size = 1000000;
    std::cout << "Test with temporary object, copy elision" << std::endl;
    SimpleVector<int> moved_vector(GenerateVector(size));
    assert(moved_vector.GetSize() == size);
    std::cout << "Done!" << std::endl << std::endl;
}

void TestTemporaryObjOperator() {
    const size_t size = 1000000;
    std::cout << "Test with temporary object, operator=" << std::endl;
    SimpleVector<int> moved_vector;
    assert(moved_vector.GetSize() == 0);
    moved_vector = GenerateVector(size);
    assert(moved_vector.GetSize() == size);
    std::cout << "Done!" << std::endl << std::endl;
}

void TestNamedMoveConstructor() {
    const size_t size = 1000000;
    std::cout << "Test with named object, move constructor" << std::endl;
    SimpleVector<int> vector_to_move(GenerateVector(size));
    assert(vector_to_move.GetSize() == size);

    SimpleVector<int> moved_vector(std::move(vector_to_move));
    assert(moved_vector.GetSize() == size);
    assert(vector_to_move.GetSize() == 0);
    std::cout << "Done!" << std::endl << std::endl;
}

void TestNamedMoveOperator() {
    const size_t size = 1000000;
    std::cout << "Test with named object, operator=" << std::endl;
    SimpleVector<int> vector_to_move(GenerateVector(size));
    assert(vector_to_move.GetSize() == size);

    SimpleVector<int> moved_vector = std::move(vector_to_move);
    assert(moved_vector.GetSize() == size);
    assert(vector_to_move.GetSize() == 0);
    std::cout << "Done!" << std::endl << std::endl;
}

void TestNoncopiableMoveConstructor() {
    const size_t size = 5;
    std::cout << "Test noncopiable object, move constructor" << std::endl;
    SimpleVector<X> vector_to_move;
    for (size_t i = 0; i < size; ++i) {
        vector_to_move.PushBack(X(i));
    }

    SimpleVector<X> moved_vector = std::move(vector_to_move);
    assert(moved_vector.GetSize() == size);
    assert(vector_to_move.GetSize() == 0);

    for (size_t i = 0; i < size; ++i) {
        assert(moved_vector[i].GetX() == i);
    }
    std::cout << "Done!" << std::endl << std::endl;
}

void TestNoncopiablePushBack() {
    const size_t size = 5;
    std::cout << "Test noncopiable push back" << std::endl;
    SimpleVector<X> v;
    for (size_t i = 0; i < size; ++i) {
        v.PushBack(X(i));
    }

    assert(v.GetSize() == size);

    for (size_t i = 0; i < size; ++i) {
        assert(v[i].GetX() == i);
    }
    std::cout << "Done!" << std::endl << std::endl;
}

void TestNoncopiableInsert() {
    const size_t size = 5;
    std::cout << "Test noncopiable insert" << std::endl;
    SimpleVector<X> v;
    for (size_t i = 0; i < size; ++i) {
        v.PushBack(X(i));
    }

    // � ������
    v.Insert(v.begin(), X(size + 1));
    assert(v.GetSize() == size + 1);
    assert(v.begin()->GetX() == size + 1);
    // � �����
    v.Insert(v.end(), X(size + 2));
    assert(v.GetSize() == size + 2);
    assert((v.end() - 1)->GetX() == size + 2);
    // � ��������
    v.Insert(v.begin() + 3, X(size + 3));
    assert(v.GetSize() == size + 3);
    assert((v.begin() + 3)->GetX() == size + 3);
    std::cout << "Done!" << std::endl << std::endl;
}

void TestNoncopiableErase() {
    const size_t size = 3;
    std::cout << "Test noncopiable erase" << std::endl;
    SimpleVector<X> v;
    for (size_t i = 0; i < size; ++i) {
        v.PushBack(X(i));
    }

    auto it = v.Erase(v.begin());
    assert(it->GetX() == 1);
    std::cout << "Done!" << std::endl << std::endl;
}

void TestNoncopiableResize() {
    const size_t size = 5;
    std::cout << "Test noncopiable erase" << std::endl;
    SimpleVector<X> v;
    for (size_t i = 0; i < size; ++i) {
        v.PushBack(X(i));
    }

    v.Resize(6);
    assert(v.GetSize() == 6);

    std::cout << "Done!" << std::endl << std::endl;
}

void TestsLauncher() {
    Test1();
    Test2();
    TestReserveConstructor();
    TestReserveMethod();
    TestMoveArrayPtr();
    TestTemporaryObjConstructor();
    TestTemporaryObjOperator();
    TestNamedMoveConstructor();
    TestNamedMoveOperator();
    TestNoncopiableMoveConstructor();
    TestNoncopiablePushBack();
    TestNoncopiableInsert();
    TestNoncopiableErase();
    TestNoncopiableResize();
}