#ifndef MODEL_RUNNER_HH
#define MODEL_RUNNER_HH

#include <vector>
#include <string>

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
  unsigned int max_machine_count_;
  unsigned int read_position_start_;
  unsigned int read_size_;

  std::vector<Result> results_ {};
  
  ModelRunner( const unsigned int method,
	       const std::string & profile_filename,
	       const std::string & client,
	       const std::string & node,
	       const unsigned int max_machine_count,
	       const double data_size_GB,
	       const unsigned int read_position_start,
	       const unsigned int read_size );

  void wait_until_finished();
  
  const std::vector<Result> results() const { return results_; }
};

#endif /* MODEL_RUNNER_HH */
