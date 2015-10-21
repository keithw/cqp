#ifndef MODEL_RUNNER_HH
#define MODEL_RUNNER_HH

#include <vector>
#include <string>
#include <thread>

#include <ext/stdio_filebuf.h>

class ModelRunner
{
public:
  struct Result
  {
    unsigned int method;
    unsigned int machine_count;
    unsigned int time_seconds;
    double cost_dollars;
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
  
  long int max_machine_count_;
  long int read_position_start_;
  long int read_size_;

  std::vector<Result> results_ {};
  POpen output_;
  
  std::thread running_thread_;
  
public:
  ModelRunner( const unsigned int method,
	       const std::string profile_filename,
	       const std::string client,
	       const std::string node,
	       const long int max_machine_count,
	       const double data_size_GB,
	       const long int read_position_start,
	       const long int read_size );

  void wait_until_finished();
  
  const std::vector<Result> results() const { return results_; }
};

#endif /* MODEL_RUNNER_HH */
