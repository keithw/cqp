#include <cstdlib>
#include <thread>
#include <chrono>

#include <unistd.h>

#include "fader.hh"

using namespace std;

int main()
{
  GTKFader fader;
  unsigned int ui_version = 0;
  
  while ( true ) {
    ui_version = fader.wait_for_event( ui_version );

    if ( fader.quit() ) {
      break;
    }
  }

  return EXIT_SUCCESS;
}
