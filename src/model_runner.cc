#include <stdexcept>
#include <numeric>

#include "model_runner.hh"
#include "exception.hh"

using namespace std;

string join( const vector<string> & command )
{
  return accumulate( command.begin() + 1, command.end(),
		     command.front(),
		     []( const string & a, const string & b )
		     { return a + " " + b; } );
}

template <typename T>
static T not_null( const string & tag, T x )
{
  if ( x == nullptr ) {
    throw unix_error( tag );
  }

  return x;
} 

void ModelRunner::POpen::Deleter::operator() ( FILE * x ) const
{
  try {
    SystemCall( "pclose", pclose( x ) );
  } catch ( const exception & e ) {
    print_exception( e ); /* don't throw from destructor */
  }
}

ModelRunner::POpen::POpen( const string & command )
  : stream_( not_null( "popen", popen( command.c_str(), "r" ) ) ),
    cpp_buf_( stream_.get(), ios::in ),
    cpp_stream_( &cpp_buf_ )
{}

ModelRunner::ModelRunner( const unsigned int method,
			  const string profile_filename,
			  const string client,
			  const string node,
			  const long int max_machine_count,
			  const double data_size_GB,
			  const long int read_position_start,
			  const long int read_size )
  : max_machine_count_( max_machine_count ),
    read_position_start_( read_position_start ),
    read_size_( read_size ),
    output_( join( { "cd models/models/method" + to_string( method ),
	"&&",
	"./point.R",
	"--machines", "../" + profile_filename,
	"--client", client,
	"--node", node,
	"--cluster-points", to_string( max_machine_count ),
	"--data", to_string( data_size_GB ),
	"--range-start", to_string( read_position_start ),
	    "--range-size", to_string( read_size ) } ) ),
    running_thread_()
{
  if ( (method != 1) and (method != 2) and (method != 4) ) {
    throw runtime_error( string( "unknown method " ) + to_string( method ) );
  }

  running_thread_ = thread( [&] () {
      string line;
      while ( output_.stream().good() and getline( output_.stream(), line ) ) {
	results_.emplace_back( Result{ method, 0, 0, 0 } );
      }
    } );
}

void ModelRunner::wait_until_finished()
{
  if ( running_thread_.joinable() ) {
    running_thread_.join();
  }
}
