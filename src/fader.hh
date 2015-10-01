#ifndef FADER_HH
#define FADER_HH

#include <memory>
#include <atomic>
#include <string>

#include "tracker.hh"

class GTKFader
{
  friend class LabeledScale;

  std::atomic<double> record_size_ { 100 };
  std::atomic<double> num_records_ { 10000 };

  std::atomic<bool> quit_ { false };

  Tracker tracker_;

  void recompute();
  
public:
  GTKFader();

  double record_size() const { return record_size_; }
  double num_records() const { return num_records_; }
  
  bool quit() const { return quit_; }

  unsigned int wait_for_event( const unsigned int known_version )
  {
    return tracker_.wait_for_event( known_version );
  }
};

#endif /* FADER_HH */
