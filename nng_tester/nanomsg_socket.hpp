#pragma once

#include <memory>
#include <string>

#include <nng/nng.h>

//===========================================================================
/// \brief Encapsulates the boilerplate of creating and connecting/binding
/// Nanomsg sockets.  Uses RAII to manage socket.
///
/// This is intentially kept light-weight so that as much dealing with Nanomsg
/// objects as possible is kept in the Nanomsg C API.
///
/// All methods in this class may throw std::runtime_error() if any of the
/// Nanomsg calls fail.
///

class NanomsgSocket
{
public:
  enum Protocol
  {
    NNG_NONE,
    NNG_PAIR,
    NNG_REQ,
    NNG_REP,
    NNG_PUB,
    NNG_SUB,
    NNG_SURVEYOR,
    NNG_RESPONDENT,
    NNG_PULL,
    NNG_PUSH,
    NNG_BUS
  };

  /// \brief RAII constructor
  ///
  /// \param protocol The scalability protocol, defined in
  ///                 the following enum

  explicit NanomsgSocket( Protocol protocol = NNG_NONE );

  /// \brief Copy constructors
  NanomsgSocket( const NanomsgSocket &other ) = default;
  NanomsgSocket &operator=( const NanomsgSocket &other ) = default;

  /// \brief RAII destructor.
  ///
  ~NanomsgSocket();

  /// \brief Bind to a url, to start listening for incoming connections.
  ///
  /// \param url The URL to bind to, e.g. tcp://*:4000, options are
  ///     <nanomsg/inproc.h> --> "inproc://some-string"
  ///     <nanomsg/ipc.h> --> "ipc://pipe-name"
  ///     <nanomsg/tcp.h> --> "tcp://host:port"
  ///     <nanomsg/ws.h>  --> "ws://host:port", default port is 80 if unset
  ///
  /// \throws std::runtime_error if there is a problem binding.
  ///
  void Bind( std::string url );

  /// \brief Connect to a socket that has called Bind().
  ///
  /// \param url The URL to connect to, e.g. tcp://127.0.0.1:4000
  ///
  /// \throws std::runtime_error if there is a problem connecting.
  ///
  void Connect( std::string url );

  // So that an instance can be passed as an argument to nng_* functions.
  operator nng_socket() const;

private:
  int protocol_;
  std::string url_;
  std::shared_ptr<nng_socket> socket_;
};
