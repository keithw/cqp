#include <iostream>
#include <cmath>

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

  for ( const Machine & machine : DrCloudMachines ) {
    const string family = input.pessimistic ? machine.pessimistic_family : machine.family;

    ModelRunner linear( 1, family, machine.best_client, machine.type, lrint( input.max_machine_count ),
			data_size_GB, read_position_start, read_size );

    linear.wait_until_finished();

      /*
    cout << "pushd models/models/method1; ./point.R --machines ../" << family << " --client " << machine.best_client << " --node " << machine.type << " --cluster-points " << lrint( input.max_machine_count ) << " --data " << data_size_GB << " --range-start " << read_position_start << " --range-size " << read_size << "; popd" << endl;
    cout << "pushd models/models/method2; ./point.R --machines ../" << family << " --client " << machine.best_client << " --node " << machine.type << " --cluster-points " << lrint( input.max_machine_count ) << " --data " << data_size_GB << " --range-start " << read_position_start << " --range-size " << read_size << "; popd" << endl;
    cout << "pushd models/models/method4; ./point.R --machines ../" << family << " --client " << machine.best_client << " --node " << machine.type << " --cluster-points " << lrint( input.max_machine_count ) << " --data " << data_size_GB << " --range-start " << read_position_start << " --range-size " << read_size << "; popd" << endl;
      */
  }
}
