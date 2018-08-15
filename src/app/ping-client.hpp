#ifndef ESP8266NDN_PING_CLIENT_HPP
#define ESP8266NDN_PING_CLIENT_HPP

#include "../core/face.hpp"

namespace ndn {

/** \brief NDN reachability test tool client side
 */
class PingClient : public PacketHandler
{
public:
  /** \brief constructor
   *  \param face a face to communication with router
   *  \param interest a prepared Interest for probe packets;
   *         typically its name should end with 'ping', and it should have MustBeFresh selector;
   *         the name may contain an initial sequence number,
   *         otherwise it must have space to add a sequence number
   *  \param pingInterval interval between probes, in millis
   *  \param pingTimeout probe timeout, in millis; default is InterestLifetime;
   *         must be less than \p pingInterval
   */
  PingClient(Face& face, InterestLite& interest, int pingInterval, int pingTimeout = -1);

  ~PingClient();

  /** \brief loop the client
   */
  void
  loop();

  /** \deprecated Calling processData is no longer necessary.
   */
  bool
  processData(const DataLite& data) __attribute__((deprecated))
  {
    return false;
  }

  /** \deprecated Calling processNack is no longer necessary.
   */
  bool
  processNack(const NetworkNackLite& nack, const InterestLite& interest) __attribute__((deprecated))
  {
    return false;
  }

  /** \brief send a probe now
   *  \note Response or timeout for previous probe will be ignored.
   */
  void
  probe();

  enum class Event {
    NONE,
    PROBE,
    RESPONSE,
    TIMEOUT,
    NACK,
  };

  typedef void (*EventCallback)(void* arg, Event evt, uint64_t seq);

  /** \brief set event handler
   *
   *  Only one handler is allowed. This overwrites any previous handler setting.
   */
  void
  onEvent(EventCallback cb, void* cbarg)
  {
    m_evtCb = cb;
    m_evtCbArg = cbarg;
  }

private:
  uint32_t
  getLastSeq() const;

  bool
  processData(const DataLite& data, uint64_t endpointId) override;

  bool
  processNack(const NetworkNackLite& nackHeader, const InterestLite& interest, uint64_t endpointId) override;

private:
  Face& m_face;
  InterestLite& m_interest;
  uint8_t m_seqBuf[9]; ///< buffer for sequence number component
  const int m_pingInterval;
  const int m_pingTimeout;
  unsigned long m_lastProbe; ///< timestamp of last probe
  bool m_isPending; ///< whether lastProbe is waiting for either response or timeout
  EventCallback m_evtCb;
  void* m_evtCbArg;
};

} // namespace ndn

#endif // ESP8266NDN_PING_CLIENT_HPP