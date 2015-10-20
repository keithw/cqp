#ifndef FADER_HH
#define FADER_HH

#include <memory>

#include "tracker.hh"
#include "input.hh"

class LabeledScale;
namespace Gtk { class Label; }

class GTKFader
{
  friend class LabeledScale;

  Input state_;
  
  std::atomic<bool> quit_ { false };

  std::unique_ptr<Gtk::Label> text_;
  
  std::unique_ptr<LabeledScale> record_size_slider_;
  std::unique_ptr<LabeledScale> num_records_slider_;
  std::unique_ptr<LabeledScale> range_start_slider_;
  std::unique_ptr<LabeledScale> range_end_slider_;  
  std::unique_ptr<LabeledScale> max_machine_count_slider_;

  Tracker tracker_;

  void recompute();
  
public:
  GTKFader();
  ~GTKFader();

  const Input & state() const { return state_; }
  
  bool quit() const { return quit_; }

  unsigned int wait_for_event( const unsigned int known_version )
  {
    return tracker_.wait_for_event( known_version );
  }
};

#endif /* FADER_HH */
