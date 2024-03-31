#include "search_server.h"
#include <cmath>
#include <numeric> // Added for accumulate
#include <string>
#include <vector>
#include <stdexcept>
#include <map>
#include <set>
#include <iostream>
#include <algorithm> // Added for none_of
#include <utility>  

SearchServer::SearchServer(const string& stop_words_text)
    : SearchServer(SplitIntoWords(stop_words_text)) {}

void SearchServer::AddDocument(int document_id, const string& document, DocumentStatus status, const vector<int>& ratings)
{
    if (document_id < 0) {
        throw invalid_argument("the document is negative");
    }

    if (documents_.count(document_id)) {
        throw invalid_argument("repeat document");
    }

    else {
        const vector<string> words = SplitIntoWordsNoStop(document);
        const double inv_word_count = 1.0 / words.size();

        for (const string& word : words) {
            if (IsValidWord(word) == false) {
                throw invalid_argument("invalid character in word");
            }
            word_to_document_freqs_[word][document_id] += inv_word_count;
        }

        documents_.emplace(document_id, DocumentData{ ComputeAverageRating(ratings), status });
        document_ids_.push_back(document_id);
    }
}
 
vector<Document> SearchServer::FindTopDocuments(const string& raw_query, DocumentStatus status) const
{
   return FindTopDocuments(
            raw_query, [status](int document_id, DocumentStatus document_status, int rating) {
                return document_status == status;
            });
}
 
vector<Document> SearchServer::FindTopDocuments(const string& raw_query) const
{
    return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
}
 
int SearchServer::GetDocumentCount() const
{
    return documents_.size();
}
 
int SearchServer::GetDocumentId(int index) const {
    return document_ids_.at(index);
}
 
tuple<vector<string>, DocumentStatus> SearchServer::MatchDocument(const string& raw_query, int document_id) const {
    const Query query = ParseQuery(raw_query);

    vector<string> matched_words;
    for (const string& word : query.plus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        if (word_to_document_freqs_.at(word).count(document_id)) {
            matched_words.push_back(word);
        }
    }
    for (const string& word : query.minus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        if (word_to_document_freqs_.at(word).count(document_id)) {
            matched_words.clear();
            break;
        }
    }

    return make_tuple(matched_words, documents_.at(document_id).status);
}
 
vector<string> SearchServer::SplitIntoWordsNoStop(const string& text) const {
    vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
}
 
int SearchServer::ComputeAverageRating(const vector<int>& ratings) {
    if (ratings.empty()) {
            return 0;
        }

        int rating_sum = accumulate(ratings.begin(), ratings.end(), 0);
        return rating_sum / static_cast<int>(ratings.size());
}
 
SearchServer::QueryWord SearchServer::ParseQueryWord(const string& word) const
{
    if (IsValidWord(word) == false) {
        throw invalid_argument("forbidden symbols");
    }
    if (word == "-") {
        throw invalid_argument("minus==word");
    }
    if (word.substr(0, 2) == "--") {
        throw invalid_argument("-- in the word");
    }

    bool is_minus = false;
    if (word[0] == '-') {
        is_minus = true;
    }

    bool is_stop = IsStopWord(word);

    // Create a new string without the first character if it is '-'
    string result = word.substr(is_minus ? 1 : 0);

    return { result, is_minus, is_stop };
}
 
bool SearchServer::IsValidWord(const string& word) {
    // A valid word must not contain special characters
    return none_of(word.begin(), word.end(), [](char c) {
        return c >= '\0' && c < ' ';
    });
}
 
 
 
SearchServer::Query SearchServer::ParseQuery(const string& text) const {
    Query query;
        for (const string& word : SplitIntoWords(text)) {
            const QueryWord query_word = ParseQueryWord(word);
            if (!query_word.is_stop) {
                if (query_word.is_minus) {
                    query.minus_words.insert(query_word.data);
                }
                else {
                    query.plus_words.insert(query_word.data);
                }
            }
        }
        return query;
}
 
// Existence required
double SearchServer::ComputeWordInverseDocumentFreq(const string& word) const {
    return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
}
    return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
}
