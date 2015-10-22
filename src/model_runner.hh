#ifndef MODEL_RUNNER_HH
#define MODEL_RUNNER_HH

#include <vector>
#include <string>
#include <memory>

#include <ext/stdio_filebuf.h>

class ModelRunner
{
public:
  struct Result
  {
    unsigned int method;
    std::string machine_type;
    std::string operation;
    int machine_count;
    double time_seconds;
    double cost_dollars;
    std::string line;
    
    Result( const unsigned int s_method,
	    const std::string & machine_type,
	    const std::vector<std::string> & fields );
    
    Result( const unsigned int s_method,
	    const std::string & machine_type,
	    const std::string & line );

    static void check_header( const unsigned int method, const std::string & line );

    static size_t operation_field( const unsigned int method );
    static size_t machine_field( const unsigned int method );
    static size_t time_field( const unsigned int method );
    static size_t cost_field( const unsigned int method );

    double bang() const { return 1.0 / ( cost_dollars * time_seconds ); }

    std::string str() const;
  };

private:
  class POpen
  {
  private:
    struct Deleter { void operator() ( FILE * x ) const; };

    std::unique_ptr<FILE, Deleter> stream_;
    __gnu_cxx::stdio_filebuf<char> cpp_buf_;
    std::istream cpp_stream_;
  
  public:
    POpen( const std::string & command );

    /* get an istream */
    std::istream & stream() { return cpp_stream_; }
  };
  
  const unsigned int method_;
  const std::string node_;
  std::vector<Result> results_ {};
  POpen output_;
  
public:
  ModelRunner( const unsigned int method,
	       const std::string profile_filename,
	       const std::string client,
	       const std::string node,
	       const long int max_machine_count,
	       const double data_size_GB,
	       const long int read_position_start,
	       const long int read_size );

  void collect_output();

  const std::vector<Result> & results() const { return results_; }
};

#endif /* MODEL_RUNNER_HH */
