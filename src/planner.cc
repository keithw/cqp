#include <iostream>
#include <cmath>
#include <list>

#include "planner.hh"
#include "machines.hh"
#include "model_runner.hh"

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

  results_.clear();
  
  for ( const Machine & machine : DrCloudMachines ) {
    const string family = input.pessimistic ? machine.pessimistic_family : machine.family;

    for ( const int method : { 1, 2, 4 } ) {
      ModelRunner model( static_cast<unsigned>( method ), family, machine.best_client, machine.type,
			 lrint( input.max_machine_count ),
			 data_size_GB, read_position_start, read_size );
      results_.insert( results_.end(), model.results().begin(), model.results().end() );
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
}

string ModelRunner::Result::str() const
{
  string ezmethod = method == 1 ? "LinearScan" : method == 2 ? "LocalIndex" : "ShuffleAll";
  return ezmethod + ", " + to_string( machine_count ) + " " + machine_type + " => " + to_string( time_seconds ) + " secs for $" + to_string( cost_dollars );
}
