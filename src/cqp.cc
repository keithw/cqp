#include <cstdlib>
#include <thread>
#include <chrono>

#include <unistd.h>

#include "fader.hh"

using namespace std;

int main()
{
  GTKFader fader;

  while ( true ) {
    if ( fader.quit() ) {
      break;
    }

    this_thread::sleep_for( chrono::milliseconds( 500 ) );
  }

  return EXIT_SUCCESS;
}
