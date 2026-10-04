#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ), stream(), isClosed(false), pushNum( 0 ), popNum( 0 ) {}

void Writer::push(string data) {
    if (is_closed() || available_capacity() == 0) {
        return;
    }
    uint64_t write_bytes = min(data.size(), available_capacity());
    if (write_bytes > 0) {
        stream.push_back(data.substr(0, write_bytes));
        pushNum += write_bytes;
    }
}

void Writer::close()
{
  isClosed = true;
}

bool Writer::is_closed() const
{
  return isClosed;
}

uint64_t Writer::available_capacity() const
{
  return capacity_ - pushNum + popNum;
}

uint64_t Writer::bytes_pushed() const
{
  return pushNum;
}

string_view Reader::peek() const
{
  if (stream.empty()) {
    return {};
  }
  return stream.front();
}

void Reader::pop( uint64_t len )
{
  uint64_t bytes_to_pop = std::min(len, bytes_buffered());
  popNum += bytes_to_pop;
  while (bytes_to_pop > 0 && !stream.empty()) {
    if (bytes_to_pop >= stream.front().size()) {
      bytes_to_pop -= stream.front().size();
      stream.pop_front();
    } else {
      stream.front().erase(0, bytes_to_pop);
      bytes_to_pop = 0;
      break;
    }
  }
}

bool Reader::is_finished() const
{
  return isClosed && stream.empty();
}

uint64_t Reader::bytes_buffered() const
{
  return pushNum - popNum;
}

uint64_t Reader::bytes_popped() const
{
  return popNum;
}

