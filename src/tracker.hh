#ifndef TRACKER_HH
#define TRACKER_HH

#include <mutex>
#include <condition_variable>

class Tracker
{
private:
  std::mutex mutex_ {};
  std::condition_variable event_ {};
  unsigned int version_ { 1 };

public:
  void mark_event();
  unsigned int wait_for_event( const unsigned int known_version );
};

#endif /* TRACKER_HH */
