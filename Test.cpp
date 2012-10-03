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

#define M_PI 3.14159265358979323846

using namespace std;

void testDFT();

int main(int argc, char **argv)
{
   testDFT();
   
   return 1;
}

void testDFT()
{
   SpeakerRecognition SR;
   
   vector<double> signal;
   double sections = (2 * M_PI) / 16;   
   double total = 0;
   
   for(int i = 0; i < 128; i++)
   {
      signal.push_back(cos(total));
      total += sections;
   }
   
   vector<double> features = SR.DFT(signal);
   
   for(unsigned int i = 0; i < features.size(); i++)
   {
      if(features[i] < 1)
         cout << 0.0 << " ";
      else
         cout << features[i] << " ";

      if(!(i % 10))
         cout << endl;
   }
   
   cout << endl;
}

