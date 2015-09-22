#include <cstdlib>
#include <thread>
#include <chrono>

#include <unistd.h>

#include "fader.hh"

using namespace std;

int main()
{
  GTKFader fader { 5 };

  while ( true ) {
    this_thread::sleep_for( chrono::milliseconds( 500 ) );

    if ( fader.quit() ) {
      break;
    }
  }

  return EXIT_SUCCESS;
}
