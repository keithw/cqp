#include <gtkmm.h>

#include <thread>
#include <string>

#include "fader.hh"

using namespace std;
using namespace Glib;
using namespace Gtk;

class LabeledScale
{
  VBox label_and_control_ {};
  Label label_ {};
  VScale control_;

public:
  LabeledScale( Container & parent, GTKFader & fader,
		const string & text, const double minval, const double maxval, const double incr,
		const double multiplier, std::atomic<double> & variable )
    : control_( minval, maxval, incr )
  {
    label_.set_markup( text );
    label_.set_padding( 10, 10 );
    control_.set_inverted();
    control_.set_value( variable * multiplier );
    control_.signal_change_value().connect_notify( [&] ( const ScrollType &, const double & val ) {
	variable = min( maxval, max( minval, val ) ) / multiplier;
	fader.recompute();
      } );

    label_and_control_.pack_start( label_, PACK_SHRINK );
    label_and_control_.pack_start( control_, PACK_EXPAND_WIDGET );

    parent.add( label_and_control_ );
  }
};

void GTKFader::recompute()
{
  /* XXX can only be called from Gtk thread and is not reentrant */
  double data_size_gigabytes = record_size() * num_records() / 1.e9;
  printf( "Data size: %.1f gigabytes\n", data_size_gigabytes );
  tracker_.mark_event();
}

GTKFader::GTKFader()
{
  thread newthread( [&] () {
      RefPtr<Application> app = Application::create();

      Window window;
      window.set_default_size( 200, 400 );
      window.set_title( "Cloud Query Designer" );
      
      VBox stack;
      window.add( stack );

      /* numerical sliders */
      HBox numeric;
      stack.pack_start( numeric );

      LabeledScale record_size( numeric, *this, "<b>record size</b> (bytes)", 1, 10000, 1, 1, record_size_ );
      LabeledScale num_records( numeric, *this, "<b>num records</b>", 1, 1e12, 1e7, 1, num_records_ );

      /* explanatory text */
      Label text;
      stack.pack_start( text, PACK_SHRINK, 10 );
      
      window.show_all();

      app->run( window, 0, nullptr );
      quit_ = true;
      tracker_.mark_event();
    } );

  newthread.detach();
}
