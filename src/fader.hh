#ifndef FADER_HH
#define FADER_HH

#include <memory>
#include <atomic>

#include "tracker.hh"

class LabeledScale;
namespace Gtk { class Label; }

class GTKFader
{
  friend class LabeledScale;

  std::atomic<double> record_size_ { 100 };
  std::atomic<double> num_records_ { 10000 };
  std::atomic<double> range_start_ { 0 };
  std::atomic<double> range_end_ { 100 };

  std::atomic<bool> quit_ { false };

  std::unique_ptr<Gtk::Label> text_;

  std::unique_ptr<LabeledScale> record_size_slider_;
  std::unique_ptr<LabeledScale> num_records_slider_;
  std::unique_ptr<LabeledScale> range_start_slider_;
  std::unique_ptr<LabeledScale> range_end_slider_;  
  
  Tracker tracker_;

  void recompute();
  
public:
  GTKFader();
  ~GTKFader();

  double record_size() const { return record_size_; }
  double num_records() const { return num_records_; }
  
  bool quit() const { return quit_; }

  unsigned int wait_for_event( const unsigned int known_version )
  {
    return tracker_.wait_for_event( known_version );
  }
};

#endif /* FADER_HH */
