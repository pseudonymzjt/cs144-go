#include "wrapping_integers.hh"
#include "debug.hh"

using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{ return Wrap32 { static_cast<uint32_t>( n ) + zero_point.raw_value_ }; }

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  int32_t diff = raw_value_ - wrap( checkpoint, zero_point ).raw_value_;
  int64_t res = static_cast<int64_t>( checkpoint ) + diff;
  return res < 0 ? res + ( 1ULL << 32 ) : static_cast<uint64_t>( res );
}