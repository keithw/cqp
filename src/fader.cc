#include <gtkmm.h>

#include <thread>
#include <string>
#include <sstream>

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
    control_.set_value_pos( POS_RIGHT );
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
  double data_size = record_size() * num_records();
  string units = "byte";

  if ( data_size >= 1e12 ) {
    data_size /= 1e12;
    units = "terabyte";
  } else if ( data_size >= 1e9 ) {
    data_size /= 1e9;
    units = "gigabyte";
  } else if ( data_size >= 1e6 ) {
    data_size /= 1e6;
    units = "megabyte";
  } else if ( data_size >= 1e3 ) {
    data_size /= 1e3;
    units = "kilobyte";
  }

  stringstream value;
  value.imbue( locale( "" ) ); /* add commas as appropriate */
  value.precision( 1 );
  value << fixed << data_size;

  string value_str = value.str();
  string plural = "s";

  if ( value_str == "1.0" ) {
    value_str = "1";
    plural = "";
  }

  text_->set_markup( "<b>data size:</b> " + value_str + " " + units + plural );
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

      text_ = make_unique<Gtk::Label>();
      record_size_slider_ = make_unique<LabeledScale>( numeric, *this, "<b>record size</b> (bytes)", 1, 200, 1, 1, record_size_ );
      num_records_slider_ = make_unique<LabeledScale>( numeric, *this, "<b>record count</b>", 1, 1e11, 1e7, 1, num_records_ );

      /* explanatory text */
      stack.pack_start( *text_, PACK_SHRINK, 10 );
      
      window.show_all();

      recompute();
      app->run( window, 0, nullptr );
      quit_ = true;
      tracker_.mark_event();
    } );

  newthread.detach();
}

GTKFader::~GTKFader() {}
