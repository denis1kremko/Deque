#include <initializer_list>
#include <algorithm>
#include <iostream>

#define BLOCK_CAPACITY 128

class Deque {
public:
    struct Block {
        int *block_data = new int[BLOCK_CAPACITY];
    };

    Deque() {
        InitializeDeque();
    }

    Deque(const Deque &rhs) {
        InitializeDeque();

        for (size_t iter = 0; iter < rhs.data_size_; ++iter) {
            PushBack(rhs[iter]);
        }
    }

    Deque(Deque &&rhs)
            : begin_(rhs.begin_),
              head_index_(rhs.head_index_),
              tail_index_(rhs.tail_index_),

              block_capacity_(rhs.block_capacity_),
              block_size_(rhs.block_size_),

              data_size_(rhs.data_size_),

              head_indent_(rhs.head_indent_),
              tail_indent_(rhs.tail_indent_) {

        rhs.InitializeDeque();
    }

    explicit Deque(size_t size) {
        InitializeDeque();

        for (size_t iter = 0; iter < size; ++iter) {
            PushBack(0);
        }
    }

    Deque(std::initializer_list<int> list) {
        InitializeDeque();

        auto list_iter = list.begin();
        for (size_t iter = 0; iter < list.size(); ++iter) {
            PushBack(*list_iter);
            ++list_iter;
        }
    }

    Deque &operator=(Deque rhs) {
        if (rhs.begin_ == begin_) {
            return *this;
        }

        DeleteData();
        InitializeDeque();

        for (size_t iter = 0; iter < rhs.data_size_; ++iter) {
            PushBack(rhs[iter]);
        }

        return *this;
    }

    void Swap(Deque &rhs) {
        Deque temp(std::move(rhs));
        rhs = std::move(*this);
        *this = std::move(temp);
    }

    void PushBack(int value) {
        if (head_indent_ == BLOCK_CAPACITY) {
            AddHead();
        }

        begin_[head_index_].block_data[head_indent_] = value;
        ++head_indent_;
        ++data_size_;
    }

    void PopBack() {
        if (head_indent_ == 0) {
            --block_size_;

            head_index_ = (head_index_ == 0) ? block_capacity_ - 1 : head_index_ - 1;
        }

        head_indent_ = (head_indent_ == 0) ? BLOCK_CAPACITY - 1 : head_indent_ - 1;
        --data_size_;
        begin_[head_index_].block_data[head_indent_] = 0;
    }

    void PushFront(int value) {
        if (tail_indent_ == BLOCK_CAPACITY) {
            AddTail();
        }

        begin_[tail_index_].block_data[tail_indent_] = value;
        tail_indent_ = (tail_indent_ == 0) ? BLOCK_CAPACITY : tail_indent_ - 1;
        ++data_size_;
    }

    void PopFront() {
        if (tail_indent_ == BLOCK_CAPACITY - 1) {
            --block_size_;

            tail_index_ = (tail_index_ + 1) % block_capacity_;
        }

        tail_indent_ = (tail_indent_ == BLOCK_CAPACITY) ? 0 : tail_indent_ + 1;
        tail_indent_ %= BLOCK_CAPACITY;
        --data_size_;
        begin_[tail_index_].block_data[tail_indent_] = 0;
    }

    int &operator[](size_t ind) {
        size_t adding = (tail_indent_ == BLOCK_CAPACITY) ? 0 : tail_indent_ + 1;
        ind += adding;
        size_t block_num = ind / BLOCK_CAPACITY;
        size_t cell_num = ind % BLOCK_CAPACITY;

        return begin_[(tail_index_ + block_num) % block_capacity_].block_data[cell_num];
    }

    int operator[](size_t ind) const {
        size_t adding = (tail_indent_ == BLOCK_CAPACITY) ? 0 : tail_indent_ + 1;
        ind += adding;
        size_t block_num = ind / BLOCK_CAPACITY;
        size_t cell_num = ind % BLOCK_CAPACITY;

        return begin_[(tail_index_ + block_num) % block_capacity_].block_data[cell_num];
    }

    size_t Size() const {
        return data_size_;
    }

    void Clear() {
        *this = Deque();
    }

    ~Deque() {
        DeleteData();
    }

private:
    void InitializeDeque() {
        begin_ = new Block[1];
        head_index_ = 0;
        tail_index_ = 0;

        block_capacity_ = 1;
        block_size_ = 1;

        data_size_ = 0;
        head_indent_ = 0;
        tail_indent_ = BLOCK_CAPACITY;
    }

    void DeleteData() {
        for (size_t block = 0; block < block_capacity_; ++block) {
            delete[](begin_ + block)->block_data;
        }
        delete[] begin_;
    }

    void AddHead() {
        if (block_size_ == block_capacity_) {
            Reallocate();
        }

        head_index_ = (head_index_ + 1) % block_capacity_;
        head_indent_ = 0;

        ++block_size_;
    }

    void AddTail() {
        if (block_size_ == block_capacity_) {
            Reallocate();
        }

        tail_index_ = (tail_index_ == 0) ? block_capacity_ - 1 : tail_index_ - 1;
        tail_indent_ = BLOCK_CAPACITY - 1;

        ++block_size_;
    }

    void Reallocate() {
        block_capacity_ *= 2;
        Block *new_tail = new Block[block_capacity_];
        for (size_t block = 0; block < block_size_; ++block) {
            delete[] new_tail[block].block_data;
            new_tail[block] = begin_[(tail_index_ + block) % block_size_];
        }

        delete[] begin_;

        begin_ = new_tail;
        head_index_ = block_size_ - 1;
        tail_index_ = 0;
    }

    Block *begin_;

    size_t head_index_;
    size_t tail_index_;

    size_t block_capacity_;
    size_t block_size_;
    size_t data_size_;

    size_t head_indent_;
    size_t tail_indent_;
};