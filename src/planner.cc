#include <iostream>

#include "planner.hh"

using namespace std;

Planner::Planner()
{}

void Planner::set_input( const Input & input )
{
  cout << input.num_records << " records" << endl;
}
