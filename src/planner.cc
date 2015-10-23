#include <iostream>
#include <cmath>
#include <list>

#include "planner.hh"
#include "machines.hh"
#include "model_runner.hh"
#include "temp_file.hh"
#include "exception.hh"

using namespace std;

Planner::Planner()
{}

void Planner::set_input( const Input & input )
{
  const long int num_records_int = lrint( input.num_records );
  const long int record_size_int = lrint( input.record_size );
  if ( record_size_int != 100 ) {
    throw runtime_error( "only record size of 100 is currently supported" );
  }
  
  const double data_size_GB = input.record_size * input.num_records / 1e9;
  const long int read_position_start = lrint(input.num_records * input.range_start / 100.0);
  const long int read_position_end = lrint(input.num_records * input.range_end / 100.0);
  const long int read_size = min( num_records_int, read_position_end - read_position_start );
  
  /* validate */
  if ( data_size_GB <= 0 ) {
    throw runtime_error( "no data" );
  }

  if ( read_position_start < 0 or read_position_start > num_records_int ) {
    throw runtime_error( "invalid read_position_start" );
  }

  if ( read_size < 0 or read_size > num_records_int ) {
    throw runtime_error( "invalid read_size" );
  }

  list<ModelRunner> models_;
  vector<ModelRunner::Result> all_results;
  
  for ( const Machine & machine : DrCloudMachines ) {
    const string family = input.pessimistic ? machine.pessimistic_family : machine.family;

    for ( const int method : { 1, 2, 4 } ) {
      models_.emplace_back( static_cast<unsigned>( method ), family, machine.best_client, machine.type,
			    lrint( input.max_machine_count ),
			    data_size_GB, read_position_start, read_size );
    }
  }

  for ( auto & model : models_ ) {
    model.collect_output();
    all_results.insert( all_results.end(), model.results().begin(), model.results().end() );
  }

  results_.clear();
  for ( const auto & x : all_results ) {
    if ( x.operation == "nth" ) {
      results_.push_back( x );
    }
  }
}

void Planner::analyze() const
{
  if ( results_.empty() ) {
    cerr << "No strategies available." << endl;
    return;
  }
  
  /* find cheapest, fastest, sweetspot */
  ModelRunner::Result cheapest = results_.front();
  ModelRunner::Result fastest  = results_.front();
  ModelRunner::Result mostbang = results_.front();

  for ( const auto & x : results_ ) {
    if ( x.cost_dollars < cheapest.cost_dollars ) {
      cheapest = x;
    }

    if ( x.time_seconds < fastest.time_seconds ) {
      fastest = x;
    }

    if ( x.bang() > mostbang.bang() ) {
      mostbang = x;
    }
  }

  cout << "Cheapest option: " << cheapest.str() << endl;
  cout << "Fastest option: " << fastest.str() << endl;
  cout << "Most bang for the buck: " << mostbang.str() << endl;

  /* now graph the results */
  
  UniqueFile data_to_plot( "/tmp/drcloud_plot" );

  for ( const auto method : { 1, 2, 4 } ) {
    for ( const auto & x : results_ ) {
      if ( static_cast<int>( x.method ) == method ) {
	const string result_line = to_string( x.time_seconds ) + " " + to_string( x.cost_dollars ) + "\n";
	data_to_plot.write( result_line );
      }
    }

    data_to_plot.write( "\n\n" );
  }

  UniqueFile gnuplot_script( "/tmp/drcloud_gnuplot" );
  gnuplot_script.write( "set xlabel 'time (s)'\n");
  gnuplot_script.write( "set ylabel 'cost ($)'\n");
  gnuplot_script.write( "set logscale xy\n" );

  /* label the special points */
  gnuplot_script.write( "set label 'Cheapest' at " + to_string( cheapest.time_seconds ) + ","
			+ to_string( cheapest.cost_dollars ) + " boxed right offset -2,-2 point lt 6 ps 2.5\n" );
  gnuplot_script.write( "set label 'Fastest' at " + to_string( fastest.time_seconds ) + ","
			+ to_string( fastest.cost_dollars ) + " boxed right offset -2,-2 point lt 6 ps 2.5\n" );
  if ( mostbang.str() != fastest.str()
       and mostbang.str() != cheapest.str() ) {
    gnuplot_script.write( "set label 'Best value' at " + to_string( mostbang.time_seconds ) + ","
			  + to_string( mostbang.cost_dollars ) + " boxed right offset -2,-2 point lt 6 ps 2.5\n" );
  }
  
  gnuplot_script.write( "plot \"" + data_to_plot.name() + "\" index 0 using 1:2 title \"LinearScan\", " );
  gnuplot_script.write( "\"\" index 1 using 1:2 title \"LocalIndex\", " );
  gnuplot_script.write( "\"\" index 2 using 1:2 title \"ShuffleAll\"\n" );

  const string command = "gnuplot --persist " + gnuplot_script.name();
  
  SystemCall( "system", system( command.c_str() ) );
}

static string human_readable_machine_name( const string & machine_type )
{
  for ( const auto & x : DrCloudMachines ) {
    if ( x.type == machine_type ) {
      return x.nice_name;
    }
  }

  throw runtime_error( string( "did not find nice name for " ) + machine_type );
}

string ModelRunner::Result::str() const
{
  string human_readable = human_readable_machine_name( machine_type );
  string ezmethod = method == 1 ? "LinearScan" : method == 2 ? "LocalIndex" : "ShuffleAll";
  return ezmethod + ", " + to_string( machine_count ) + " " + human_readable + " => " + to_string( time_seconds ) + " secs for $" + to_string( cost_dollars );
}
