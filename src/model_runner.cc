#include <stdexcept>
#include <numeric>
#include <ext/stdio_filebuf.h>

#include "model_runner.hh"
#include "exception.hh"

using namespace std;

static string join( const vector<string> & command )
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

class POpen
{
private:
  struct Deleter
  {
    void operator() ( FILE * x ) const
    {
      try {
	SystemCall( "pclose", pclose( x ) );
      } catch ( const exception & e ) {
	print_exception( e ); /* don't throw from destructor */
      }
    }
  };

  unique_ptr<FILE, Deleter> stream_;
  __gnu_cxx::stdio_filebuf<char> cpp_buf_;
  istream cpp_stream_;
  
public:
  POpen( const std::string & command )
    : stream_( not_null( "popen", popen( command.c_str(), "r" ) ) ),
      cpp_buf_( stream_.get(), ios::in ),
      cpp_stream_( &cpp_buf_ )
  {
    cerr << "running: " << command << endl;
  }

  /* get an istream */
  istream & stream() { return cpp_stream_; }
};

ModelRunner::ModelRunner( const unsigned int method,
			  const string & profile_filename,
			  const string & client,
			  const string & node,
			  const long int max_machine_count,
			  const double data_size_GB,
			  const long int read_position_start,
			  const long int read_size )
  : max_machine_count_( max_machine_count ),
    read_position_start_( read_position_start ),
    read_size_( read_size ),
    running_thread_( [=] () {
	try {
	  if ( (method != 1) and (method != 2) and (method != 4) ) {
	    throw runtime_error( "unknown method " + method );
	  }

	  const vector<string> command_vec = { "cd models/models/method" + to_string( method ) + "&&",
					       "./point.R",
					       "--machines", "../" + profile_filename,
					       "--client", client,
					       "--node", node,
					       "--cluster-points", to_string( max_machine_count ),
					       "--data", to_string( data_size_GB ),
					       "--range-start", to_string( read_position_start ),
					       "--range-size", to_string( read_size ) };

	  POpen results( join( command_vec ) );

	  string line;
	  while ( results.stream().good() and getline( results.stream(), line ) ) {
	    cerr << "got results: " << line << endl;
	  }
	} catch ( ... ) {
	  thread_exception_ = current_exception();
	}
      } )
{}

void ModelRunner::wait_until_finished()
{
  running_thread_.join();

  if ( thread_exception_ != exception_ptr() ) {
    try {
      rethrow_exception( thread_exception_ );
    } catch ( const exception & e ) {
      cerr << "ModelRunner exited from exception: ";
      print_exception( e );
    }
  }
}
