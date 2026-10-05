#include "reassembler.hh"
#include "debug.hh"

using namespace std;

void Reassembler::_merge_operation(uint64_t first_index, string data) {
  if(data.empty()) return;
  auto it = unassembled_.upper_bound(first_index);
  if(it != unassembled_.begin()) {
    auto prev = std::prev(it);
    uint64_t prevEnd = prev->first + prev->second.length();
    if(prevEnd >= first_index) {
      if(prevEnd >= first_index + data.length()) return;
      data = prev->second + data.substr( prevEnd - first_index );
      first_index = prev->first;
      bytes_pending_ -= prev->second.size();
      unassembled_.erase(prev);
    }
  }

  while (it != unassembled_.end() && it->first <= first_index + data.size()) {
    uint64_t end = it->first + it->second.size();
    if(end > first_index + data.length()) data += it->second.substr(first_index + data.length() - it->first);
    bytes_pending_ -= it->second.size();
    it = unassembled_.erase(it);
  }

  bytes_pending_ += data.size();
  unassembled_[first_index] = std::move( data );
}

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  uint64_t buffer_start = output_.writer().bytes_pushed();
  uint64_t buffer_end = buffer_start + output_.writer().available_capacity();

  if (is_last_substring) {
    has_eof_ = true;
    eof_index_ = first_index + data.length();
  }
  if (first_index >= buffer_end || first_index + data.length() <= buffer_start) {
    if (has_eof_ && output_.writer().bytes_pushed() == eof_index_) {
      output_.writer().close();
    }
    return;
  }

  if (first_index + data.length() > buffer_end) {
    data.resize(buffer_end - first_index);
  }
  if (first_index < buffer_start) {
    data = data.substr(buffer_start - first_index);
    first_index = buffer_start; 
  }

  _merge_operation(first_index, data);

  while (!unassembled_.empty() && unassembled_.begin()->first == output_.writer().bytes_pushed()) {
    auto it = unassembled_.begin();
    output_.writer().push(it->second);
    bytes_pending_ -= it->second.size();
    unassembled_.erase(it);
  }

  if (has_eof_ && output_.writer().bytes_pushed() == eof_index_) {
    output_.writer().close();
  }
}

// How many bytes are stored in the Reassembler itself?
// This function is for testing only; don't add extra state to support it.
uint64_t Reassembler::count_bytes_pending() const
{
  return bytes_pending_;
}
