// Travis Rotz

#ifndef SPEAKERRECOGNITION_H
#define SPEAKERRECOGNITION_H

#include <iostream>
#include <algorithm>
#include <complex>
#include <vector>
#include <iomanip>
#include <sstream>

//#define NDEBUG
#include <assert.h>

#define LRG_NUM 1000000000 // Used in place of infinity in Dynamic Time Warping Algorithm
#define M_PI 3.14159265358979323846 // PI
#define WINDOW 15 // Window size used in Dynamic Time Warping Algorithm
#define FILE "Speakers" // Contains the names of speakers with wav files available
#define POINTS 1024 // Points used in DFT algorithm

using namespace std;

/******************************************************************************
 * SpeakerRecognition Class
 *
 * Performs all functions related to Recognition
 * 
 *****************************************************************************/
class SpeakerRecognition
{

public:

   SpeakerRecognition()
   {

   }
   
   /**************************************************************************
    * Opens FILE. This file contains the names of speakers with wav files
    * available. Opens each wav file, reads the contents, performs
    * Transform, creates a Speaker based on features extracted.
    * 
    * If a Speaker already has Transform data stored, does not recalculate.
    **************************************************************************/
   void setup()
   {
      mSpeakers.clear();
      fstream SRFile(FILE, ios::in);
      assert(SRFile.good());
      
      string speakerName;

      cout << "Retrieving speaker information" << endl;

      //If a speaker already has features saved, read them
      while(SRFile >> speakerName)
         mSpeakers.push_back(Speaker(speakerName));

      SRFile.close();

      cout << endl;
      cout << "Extracting features" << endl;
      
      //If a speaker does NOT have features saved, create them
      for(unsigned int i = 0; i < mSpeakers.size(); i++)
      {
         if( mSpeakers[i].featureSize() == 0 ) // If no features
         {
            vector<double> signal = getData(mSpeakers[i].getName());
            
            vector< vector<double> > features = Transform(signal);
         
            mSpeakers[i].setFeatures(features);
            cout << mSpeakers[i].getName() << " features successfully extracted" << endl;
         }
         else
            cout << "No features to extract" << endl;
      }
      
      cerr << endl << endl;
   }

   /**************************************************************************
    * Calls the save function of each Speaker
    **************************************************************************/
   void save()
   {
      cout << "Saving Speakers" << endl;

      vector<Speaker>::iterator it;

      for(it = mSpeakers.begin(); it != mSpeakers.end(); it++)
         it->save();
   }
   
   /**************************************************************************
    * Replaces contents of all .ros files in FILE with "0 0"
    * All tranforms will be recalculated and stored.
    **************************************************************************/
   void reset()
   {
      fstream SRFile(FILE, ios::in);
      assert(SRFile.good());

      string name;
      while(SRFile >> name)
      {
         string file = name + ".ros";
         fstream f(file.c_str(), ios::out);
         assert(f.good());
         f << "0" << endl;
         f << "0" << endl;
         f.close();
      }

      SRFile.close();
   }
   
   /**************************************************************************
    * Returns the signal from 'name'.wav
    **************************************************************************/
   vector<double> getData(string name)
   {
      string file = name + ".wav";
      fstream f(file.c_str(), ios::in | ios::binary);
      assert(f.good());
      
      char buffer[4];
   
      // Dynamically find where voice data starts
      while(true)
      {
         while(f.peek() != 'd')
            f.get();
         
         f.read(buffer, 4);
         
         if(strncmp(buffer, "data", 4) == 0)
            break;
      }
      
      f.read(buffer, 4);

      // Performs Endian conversion (Only works with RIFF wav files)
      unsigned int numSamples = *reinterpret_cast<unsigned int*>(buffer) / 2;

      vector<double>signal;

      char buffer2[2];

      // Get all data values
      for(unsigned int i = 0; i < numSamples; i++)
      {
         f.read(buffer2, 2);

         // Performs Endian conversion (Only works with RIFF wav files)
         signal.push_back(*reinterpret_cast<short*>(buffer2));
      }
      
      f.close();

      cout << name << "'s voice data successfully input" << endl;

      return signal;
   }
   
   /***************************************************************************
    * Discrete Fourier Transform
    * 
    * Returns the fourier transform of 'signal'
    * 
    * SOURCE: vilimpoc.org/research/snippets/uc-dft/
    * SOURCE: www.arachnoid.com/signal_processing/dft.html
    * 
    * X(k) = n=0 -> N-1 xn e ^ -2 * i * Pi * k/N * n
    **************************************************************************/
   vector<double> DFT(vector<double> signal)
   {
      int N = signal.size();
      double* real = new double[N];
      double* imag = new double[N];
      
      for (int k = 0; k < (N / 2); k++) // Second half is a mirror of first
      {
         double realTemp = 0.0;
         double imagTemp = 0.0;

         for (int n = 0; n < N; n++) // Perform algorithm on entire sample space
         {
            double a = 2.0 * M_PI * n * k / N;
            
            realTemp += signal[n] * cos(a) * (N - 1);
            imagTemp += signal[n] * sin(a) * (N - 1);
         }
         
         real[k] = realTemp;
         imag[k] = imagTemp;
      }
      
      vector<double> results;
      
      // Compile magnitudes of waveform
      for(int i = 1; i < (N / 2); i++) // Get rid of DC value at 0
         results.push_back( sqrt(real[i] * real[i] + imag[i] * imag[i]) );
         
      return results;
   }
   
   /**************************************************************************
    * Returns 2 dimensional array of featrues extracted from 'signal'.
    * 
    * 
    **************************************************************************/
   vector< vector<double> > Transform(vector<double> signal)
   {
      assert(signal.size() > POINTS);
   
      vector< vector<double> > features; // Returned
      
      // Cuts off samples if POINTS is not a divisor
      double numPoints = signal.size() / POINTS; // Zero window overlap
      
      vector<double>::iterator sigIt = signal.begin();
      
      // Perform Transform on all samples in POINTS chunks
      for(double i = 0; i < numPoints; i++)
      {
         vector<double> feature;
         
         for(int j = 0; j < POINTS; j++)
         {
            feature.push_back(*sigIt);
            sigIt++;
         }
         
         //sigIt -= POINTS / 2;
         
         features.push_back( DFT(feature) );
      }
      
      return features;
   }
   
   /**************************************************************************
    * Outputs the speaker's name whose most like 'name'
    * Does not save 'name's speaker data
    **************************************************************************/
   void checkSpeaker(string name)
   {
      vector<double> signal = getData(name);
      vector< vector<double> > newFeatures = Transform(signal);
      
      double difference = 0;
      double lowest = LRG_NUM;
      string speakerName = "";
      
      cout << setw(15) << "Names" << setw(15) << "Differences" << endl;
      
      // Compare every speaker in database with 'name'
      for(unsigned int i = 0; i < mSpeakers.size(); i++)
      {
         vector< vector<double> > oldFeatures = mSpeakers[i].getFeatures();
         
         // DTW breaks down if second vector is longer than first
         if( newFeatures.size() >= oldFeatures.size() )
            difference = DTW(newFeatures, oldFeatures);
         else
            difference = DTW(oldFeatures, newFeatures);
            
         // If new closest match, then replace
         if(difference < lowest)
         {
            lowest = difference;
            speakerName = mSpeakers[i].getName();
         }
         
         cout << setw(15) << mSpeakers[i].getName() << setw(15) << difference << endl;
      }
      
      cout << endl << name + " == " + speakerName << endl << endl;
   }
   
   /**************************************************************************
    * Returns the total difference between the 2 dimensional vectors.
    * Always works when first vector's length is greater than or equal to the
    * second's
    * Dynamic Time Warping Algorithm
    * SOURCE: en.wikipedia.org/wiki/Dynamic_time_warping
    **************************************************************************/
   double DTW(vector< vector<double> > one, vector< vector<double> > two)
   {
      assert(one.size() >= two.size());
      
      // Declare
	   int rows, cols, i, j, win;
	   double cost;
	   
	   // Set
      rows = one.size();
      cols = two.size();
	   
	   // Declare, allocate, and set values to LRG_NUM
	   vector< vector<double> > DTW(rows, vector<double>(cols, LRG_NUM));
   	      
   	DTW[0][0] = 0;
      
      // Set window size
      win = max( WINDOW, (int)(max(rows, cols) - min(rows, cols)) );

      // Algorithm
      for(i = 1; i < rows; i++)
         for(j = max(1, i - win); j < min(cols, i + win); j++)
         {
            double pearson = getPearson(one[i], two[j]);// 1 & -1 are perfect matches // 0 is completely opposite
            
            assert(pearson < 1 && pearson > -1); // If a perfect match, you are comparing the same files
            
            cost = 1 - abs(pearson); // The lower the cost, the more alike
            DTW[i][j] = cost + min(DTW[i - 1][j], min(DTW[i][j - 1], DTW[i - 1][j - 1]));
         }

      double result = DTW[rows - 1][cols - 1];
      
      return result;
   }
   
   /**************************************************************************
    * Computes the cross correlation coefficient, which is the difference
    * between the waveforms x & y.
    * SOURCE: www.codeproject.com/Articles/49723/Linear-correlation-and-statistical-functions
    **************************************************************************/
   double getPearson(vector<double> x, vector<double> y)
   {
      assert(x.size() == y.size());
      
      //will regularize the unusual case of complete correlation
      const double TINY = 1.0e-20;
      int j, n = x.size();
      double yt, xt;
      double syy = 0.0, sxy = 0.0, sxx = 0.0, ay = 0.0, ax = 0.0;
      
      for (j = 0; j < n; j++)
      {
         //finds the mean
         ax += x[j];
         ay += y[j];
      }
      
      ax /= n;
      ay /= n;
      
      for (j = 0; j < n; j++)
      {
         // compute correlation coefficient
         xt = x[j] - ax;
         yt = y[j] - ay;
         sxx += xt * xt;
         syy += yt * yt;
         sxy += xt * yt;
      }

      return sxy / (sqrt(sxx * syy) + TINY);
   }

private:
   vector<Speaker> mSpeakers;
   
};

#endif

