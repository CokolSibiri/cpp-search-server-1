#pragma once

#include <vector>
#include <iostream>


template <typename Iterator>
class IteratorRange {
public:
    IteratorRange(Iterator begin, Iterator end) : begin_{begin}, end_{end} {}

    Iterator begin() {
        return begin_;
    }

    Iterator end() {
        return end_;
    }

    size_t size() {
        return end_ - begin_;
    }

private:
    Iterator begin_;
    Iterator end_;
};

template <typename Iterator>
std::ostream& operator<<(std::ostream& output, IteratorRange<Iterator> iterator_range) {
    using namespace std::literals;
    for (auto ir = iterator_range.begin(); ir != iterator_range.end(); ++ir ) {
    output << "{ document_id = "s << ir->id << ", relevance = "s << ir->relevance << ", rating = "s << ir->rating << " }"s;
    }
    return output;
}

template <typename Iterator>
bool operator==(IteratorRange<Iterator> left, IteratorRange<Iterator> right) {
    return left.begin() == right.begin() &&
           left.end() == right.end();
}

template <typename Iterator>
class Paginator {
public:
    Paginator(Iterator begin, Iterator end, size_t page_size) {
        Iterator b = begin;

        while (b + page_size < end) {
            pages_.push_back(IteratorRange<Iterator>(b, b + page_size));
            b += page_size;
        }
        if (b < end) {
            pages_.push_back(IteratorRange<Iterator>(b, end));
        }
    }

    typename std::vector<IteratorRange<Iterator>>::iterator begin() {
        return pages_.begin();
    }

    typename std::vector<IteratorRange<Iterator>>::iterator end() {
        return pages_.end();
    }

    typename std::vector<IteratorRange<Iterator>>::const_iterator begin() const {
        return pages_.cbegin();
    }

    typename std::vector<IteratorRange<Iterator>>::const_iterator end() const  {
        return pages_.cend();
    }

private:
    std::vector<IteratorRange<Iterator>> pages_;
};

template <typename Container>
auto Paginate(const Container& c, size_t page_size) {
    return Paginator(c.begin(), c.end(), page_size);
}