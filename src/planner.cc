#include <iostream>
#include <cmath>

#include "planner.hh"
#include "machines.hh"

using namespace std;

Planner::Planner()
{}

void Planner::set_input( const Input & input )
{
  if ( input.record_size != 100 ) {
    throw runtime_error( "only record size of 100 is currently supported" );
  }

  const double data_size_GB = input.record_size * input.num_records / 1e9;
  const long int read_position_start = lrint( input.num_records * input.range_start / 100.0 );
  const long int read_position_end = lrint( input.num_records * input.range_end / 100.0 );
  const long int read_size = read_position_end - read_position_start;
  
  /* validate */
  if ( data_size_GB <= 0 ) {
    throw runtime_error( "no data" );
  }

  if ( read_position_start < 0 or read_position_start > input.num_records ) {
    throw runtime_error( "invalid read_position_start" );
  }

  if ( read_size < 0 or read_size > input.num_records ) {
    throw runtime_error( "invalid read_position_end" );
  }

  for ( const Machine & machine : DrCloudMachines ) {
    cout << "./point.R --machines ../" << machine.family << "--client i2.8x --node " << machine.type << " --cluster-points 128 --data " << input.num_records << " --range-start " << read_position_start << " --range-size " << read_size << endl;
  }
}
