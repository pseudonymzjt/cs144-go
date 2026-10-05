#include "tcp_receiver.hh"
#include "debug.hh"

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message )
{
  if ( message.RST ) {
    reader().set_error();
    return;
  }

  if ( !has_syn_ ) {
    if ( !message.SYN ) {
      return;
    }
    zero_point_ = message.seqno; // 记录 ISN
    has_syn_ = true;
  }

  uint64_t checkpoint = reassembler_.writer().bytes_pushed();
  uint64_t abs_seqno = message.seqno.unwrap( zero_point_, checkpoint );
  uint64_t first_index = message.SYN ? 0 : abs_seqno - 1;

  reassembler_.insert( first_index, std::move( message.payload ), message.FIN );
}

TCPReceiverMessage TCPReceiver::send() const
{
  TCPReceiverMessage msg;

  msg.RST = reader().has_error();

  uint64_t capacity = writer().available_capacity();
  msg.window_size = static_cast<uint16_t>( std::min( capacity, static_cast<uint64_t>( UINT16_MAX ) ) );

  if ( has_syn_ ) {
    uint64_t abs_ackno = 1 + writer().bytes_pushed();
    if ( writer().is_closed() ) {
      abs_ackno += 1;
    }
    msg.ackno = Wrap32::wrap( abs_ackno, zero_point_ );
  }

  return msg;
}
