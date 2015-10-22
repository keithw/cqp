#include <cstdlib>
#include <thread>
#include <chrono>
#include <iostream>

#include <unistd.h>

#include "fader.hh"
#include "planner.hh"

using namespace std;

int main()
{
  GTKFader fader;
  Planner planner;
  
  unsigned int ui_version = 0;
  
  while ( true ) {
    ui_version = fader.wait_for_event( ui_version );

    if ( fader.quit() ) {
      break;
    }

    cerr << "updating... ";
    planner.set_input( fader.state() );
    cerr << "done." << endl;
    planner.analyze();
    planner.graph();
  }

  return EXIT_SUCCESS;
}
