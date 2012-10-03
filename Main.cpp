/******************************************************************************
 * Senior Project
 * Travis Rotz
 * 6 April 2012
 *****************************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <vector>
#include <math.h>
#include <fftw3.h>
#include <sstream>
#include <stdlib.h>

#include "Speaker.h"
#include "SpeakerRecognition.h"

using namespace std;

int main(int argc, char **argv)
{ 
   
   if(argc < 1 || argc > 1)
   {
      cerr << "ERROR: <No Arguments>" << endl;
      return 0;
   }

   SpeakerRecognition SR;
   //SR.reset();
   SR.setup();
   
   //SR.checkSpeaker("BroNeff2");
   //SR.checkSpeaker("BroTwitchell3");
   //SR.checkSpeaker("JoeDzado2");
   //SR.checkSpeaker("TravisRotz2");
   //SR.checkSpeaker("Becca2");
   //SR.checkSpeaker("Dad2");
   //SR.checkSpeaker("Christa2");
   //SR.checkSpeaker("Jennie2");
   //SR.checkSpeaker("Greg2");
   //SR.checkSpeaker("Jon3");
   SR.checkSpeaker("Cindy2");

   SR.save();

   return 1;
}

