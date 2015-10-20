#ifndef INPUT_HH
#define INPUT_HH

#include <atomic>

struct Input
{
  std::atomic<double> record_size { 100 };
  std::atomic<double> num_records { 10000 };
  std::atomic<double> range_start { 0 };
  std::atomic<double> range_end { 100 };
  std::atomic<bool> pessimistic { false };
};

#endif /* INPUT_HH */
