#include <nng/nng.h>

#include "nanomsg_socket.hpp"

#include <nng/protocol/pair0/pair.h>
#include <nng/protocol/reqrep0/rep.h>
#include <nng/protocol/reqrep0/req.h>
#include <nng/protocol/pubsub0/pub.h>
#include <nng/protocol/pubsub0/sub.h>
#include <nng/protocol/survey0/survey.h>
#include <nng/protocol/survey0/respond.h>
#include <nng/protocol/pipeline0/pull.h>
#include <nng/protocol/pipeline0/push.h>
#include <nng/protocol/bus0/bus.h>

//== Class: NanomsgSocket ===================================================
NanomsgSocket::NanomsgSocket( Protocol protocol ) :
    protocol_( protocol ),
    url_( "" ),
    socket_( std::make_unique<nng_socket>() )
{
  int rc{ 0 };

  switch (protocol)
  {
  case NNG_NONE:
    break;
  case NNG_PAIR:
    rc = nng_pair_open( socket_.get() );
    break;
  case NNG_REQ:
    rc = nng_req_open( socket_.get() );
    break;
  case NNG_REP:
    rc = nng_rep_open( socket_.get() );
    break;
  case NNG_PUB:
    rc = nng_pub_open( socket_.get() );
    break;
  case NNG_SUB:
    rc = nng_sub_open( socket_.get() );
    break;
  case NNG_SURVEYOR:
    rc = nng_surveyor_open( socket_.get() );
    break;
  case NNG_RESPONDENT:
    rc = nng_respondent_open( socket_.get() );
    break;
  case NNG_PULL:
    rc = nng_pull_open( socket_.get() );
    break;
  case NNG_PUSH:
    rc = nng_push_open( socket_.get() );
    break;
  case NNG_BUS:
    rc = nng_bus_open( socket_.get() );
    break;
  default:
    break;
  }
}

//---------------------------------------------------------------------------
NanomsgSocket::~NanomsgSocket()
{
  if( socket_.use_count() == 1 )  // last instance of this socket
  {
    nng_close( *socket_ );
  }
}

//---------------------------------------------------------------------------
void NanomsgSocket::Bind( std::string url )
{
  url_ = url;

  int rc = nng_listen( *socket_, url_.c_str(), NULL, 0 );
}

//---------------------------------------------------------------------------
void NanomsgSocket::Connect( std::string url )
{
  // Non-blocking is needed as the listener may not be ready 
  url_ = url;
  int rc = nng_dial( *socket_, url_.c_str(), NULL, NNG_FLAG_NONBLOCK );
}

//---------------------------------------------------------------------------
NanomsgSocket::operator nng_socket() const { return *socket_; }
