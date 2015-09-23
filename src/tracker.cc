#include "tracker.hh"

using namespace std;

void Tracker::mark_event()
{
  {
    unique_lock<mutex> lock { mutex_ };
    version_++;
  }

  event_.notify_all();
}

unsigned int Tracker::wait_for_event( const unsigned int known_version )
{
  unique_lock<mutex> lock { mutex_ };

  event_.wait( lock, [&] { return version_ > known_version; } );

  return version_;
}
