#ifndef MODEL_RUNNER_HH
#define MODEL_RUNNER_HH

#include <vector>
#include <string>
#include <thread>

class ModelRunner
{
public:
  struct Result
  {
    unsigned int machine_count;
    unsigned int time_seconds;
    double cost_dollars;
  };

private:
  long int max_machine_count_;
  long int read_position_start_;
  long int read_size_;
  std::exception_ptr thread_exception_ {};
  std::thread running_thread_;

  std::vector<Result> results_ {};

public:
  ModelRunner( const unsigned int method,
	       const std::string & profile_filename,
	       const std::string & client,
	       const std::string & node,
	       const long int max_machine_count,
	       const double data_size_GB,
	       const long int read_position_start,
	       const long int read_size );

  void wait_until_finished();
  
  const std::vector<Result> results() const { return results_; }
};

#endif /* MODEL_RUNNER_HH */
