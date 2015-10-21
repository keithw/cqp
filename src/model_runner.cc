#include <stdexcept>
#include <numeric>
#include <algorithm>
#include <cstdlib>

#include "model_runner.hh"
#include "exception.hh"
#include "tokenize.hh"

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
  : output_( join( { "cd models/models/method" + to_string( method ),
	  "&&",
	  "./point.R",
	  "--machines", "../" + profile_filename,
	  "--client", client,
	  "--node", node,
	  "--cluster-points", to_string( max_machine_count ),
	  "--data", to_string( data_size_GB ),
	  "--range-start", to_string( read_position_start ),
	  "--range-size", to_string( read_size ) } ) )
{
  if ( (method != 1) and (method != 2) and (method != 4) ) {
    throw runtime_error( string( "unknown method " ) + to_string( method ) );
  }

  bool first_line = true;
  string line;
  while ( output_.stream().good() and getline( output_.stream(), line ) ) {
    if ( first_line ) {
      Result::check_header( method, line );
      first_line = false;
    } else {
      results_.emplace_back( method, node, line );
    }
  }
}

void ModelRunner::Result::check_header( const unsigned int method, const std::string & line )
{
  if ( (method != 1) and (method != 2) and (method != 4) ) {
    throw runtime_error( string( "unknown method in check_header " ) + to_string( method ) );
  }

  vector<string> fields = split( line, " " );
  remove_if( fields.begin(), fields.end(),
	     [] ( const string & x ) { return x.empty(); } );

  try {
    if ( fields.at( operation_field( method ) ) != "operation" ) {
      throw runtime_error( "unexpected field heading (expecting operation)" );
    }

    if ( fields.at( machine_field( method ) ) != "nodes" ) {
      throw runtime_error( "unexpected field heading (expecting nodes)" );
    }

    if ( fields.at( time_field( method ) ) != "time.total" ) {
      throw runtime_error( string( "unexpected field heading (expecting time.total)" ) );
    }

    if ( fields.at( cost_field( method ) ) != "cost" ) {
      throw runtime_error( "unexpected field heading (expecting cost)" );
    }
  } catch ( ... ) {
    cerr << "method " << method << ", bad line: " << line << endl;
    throw;
  }
}

static vector<string> split_and_remove( const string & line )
{
  vector<string> fields = split( line, " " );
  remove_if( fields.begin(), fields.end(),
	     [] ( const string & x ) { return x.empty(); } );  
  return fields;
}

ModelRunner::Result::Result( const unsigned int s_method,
			     const string & s_machine_type,
			     const string & line )
  : Result( s_method, s_machine_type, split_and_remove( line ) )
{}

ModelRunner::Result::Result( const unsigned int s_method,
			     const string & s_machine_type,
			     const vector<string> & fields )
  :  method( s_method ),
     machine_type( s_machine_type ),
     operation( fields.at( operation_field( method ) ) ),
     machine_count( atoi( fields.at( machine_field( method ) ).c_str() ) ),
     time_seconds( atof( fields.at( time_field( method ) ).c_str() ) ),
     cost_dollars( atof( fields.at( cost_field( method ) ).c_str() ) ),
     line( join( fields ) )
{
  if ( machine_count < 1 or time_seconds < 0 or cost_dollars < 0 ) {
    throw runtime_error( "invalid result" );
  }

  if ( (method != 1) and (method != 2) and (method != 4) ) {
    throw runtime_error( string( "unknown method " ) + to_string( method ) );
  }
}
  
size_t ModelRunner::Result::operation_field( const unsigned int )
{
  return 0;
}

size_t ModelRunner::Result::machine_field( const unsigned int )
{
  return 1;
}

size_t ModelRunner::Result::time_field( const unsigned int method )
{
  return method == 4 ? 5 : 4;
}

size_t ModelRunner::Result::cost_field( const unsigned int method )
{
  return method == 1 ? 9 : 10;
}
