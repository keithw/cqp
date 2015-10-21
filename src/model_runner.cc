#include <stdexcept>

#include "model_runner.hh"

using namespace std;

ModelRunner::ModelRunner( const unsigned int method,
			  const string & profile_filename,
			  const string & client,
			  const string & node,
			  const unsigned int max_machine_count,
			  const double data_size_GB,
			  const unsigned int read_position_start,
			  const unsigned int read_size )
  : max_machine_count_( max_machine_count ),
    read_position_start_( read_position_start ),
    read_size_( read_size )
{
  if ( method != 1 and method != 2 and method != 4 ) {
    throw runtime_error( "unknown method " + method );
  }

  const vector<string> command = { "./point.R",
				   "--machines", "../" + profile_filename,
				   "--client", client,
				   "--node", node,
				   "--cluster-points", to_string( max_machine_count ),
				   "--data", to_string( data_size_GB ),
				   "--range-start", to_string( read_position_start ),
				   "--read-size", to_string( read_size ) };
}
