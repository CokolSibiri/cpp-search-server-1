#pragma once

#include <vector>
#include <string>
#include <deque>

#include "document.h"
#include "search_server.h"

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server) : search_server_(search_server) {};
    
    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate);
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);
    std::vector<Document> AddFindRequest(const std::string& raw_query);

    int GetNoResultRequests() const;

private:
    struct QueryResult {
        size_t matched_documents_number;       
        size_t day;
    };

    std::deque<QueryResult> requests_;
    const static int min_in_day_ = 1440;
    const SearchServer& search_server_;
    size_t time_ = 0;
    size_t day_counter_ = 1;
};

template <typename DocumentPredicate>
std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {    
    ++time_;
    if (time_ == min_in_day_ + 1) {
        time_ = 1;
        ++day_counter_;
    }

    if (requests_.front().day < day_counter_ && !requests_.empty()) {
        requests_.pop_front();
    }

    auto result = search_server_.FindTopDocuments(raw_query, document_predicate);
    if (result.empty()) {
        requests_.push_back({result.size(), day_counter_});
    }
    return result;
}