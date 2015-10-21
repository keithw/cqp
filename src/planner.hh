#ifndef PLANNER_HH
#define PLANNER_HH

#include "input.hh"
#include "model_runner.hh"

class Planner
{
private:
  std::vector<ModelRunner::Result> results_ {};

public:
  Planner();

  void set_input( const Input & input );
  void analyze() const;
};

#endif /* PLANNER_HH */
