// Travis Rotz

#ifndef SPEAKER_H
#define SPEAKER_H

#include <string>
#include <vector>
#include <fstream>

//#define NDEBUG
#include <assert.h>

using namespace std;

/******************************************************************************
 * Speaker Class
 * 
 * Saves and restores name and features of a speaker
 *****************************************************************************/
class Speaker {

public:

   /******************************************************************************
    * Default Constructor
    *****************************************************************************/
   Speaker()
   {
      mSpeakerName = "";
   }

   /******************************************************************************
    * Constructor
    * If no file on record of 'name', then do nothin
    * If file on record of 'name', then import features
    *****************************************************************************/
   Speaker(string name)
   {
      mSpeakerName = name;
      
      string file = name + ".ros";
      fstream f(file.c_str(), ios::in);
      
      int numFeatures = 0;

      if(f.good())
      {
         f >> numFeatures;
      
         int featureSize;
         f >> featureSize;
      
         for(int i = 0; i < numFeatures; i++)
         {
            vector<double> feature;
         
            for(int j = 0; j < featureSize; j++)
            {
               double temp;
               f >> temp;
               feature.push_back(temp);
            }
         
            mFeatures.push_back(feature);
         }
      }

      cout << mSpeakerName;
      if(numFeatures > 0)
         cout << "'s name and features successfully retrieved" << endl;
      else
         cout << " has no features to retrieve" << endl;
      
      f.close();
   }

   /******************************************************************************
    * Export features to file named mSpeakerName'.ros'
    *****************************************************************************/
   void save()
   {
      if(!mFeatures.empty())
      {
         string file = mSpeakerName + ".ros";
         fstream f(file.c_str(), ios::out);
      
         f << mFeatures.size() << endl;
      
         int featureSize = mFeatures[0].size();
         f << featureSize << endl;
      
         for(unsigned int i = 0; i < mFeatures.size(); i++)
         {
            for(int j = 0; j < featureSize; j ++)
               f << mFeatures[i][j] << " ";
         
            f << endl;
         }

         cout << mSpeakerName << " stored successfully" << endl;
      
         f.close();
      }
      else
         cerr << mSpeakerName << " has nothing to store" << endl;
   }
   
   /******************************************************************************
    * Assignment operator for Speaker
    *****************************************************************************/
   Speaker & operator=(const Speaker &rhs)
   {
      // If the exact same object, no need to assign
      if (this == &rhs)
         return *this;

      mSpeakerName = rhs.mSpeakerName;
      mFeatures = rhs.mFeatures;

      return *this;
   }
   
   /******************************************************************************
    * Return the name of the speaker
    *****************************************************************************/
   string getName()
   {
      return mSpeakerName;
   }
   
   /******************************************************************************
    * Return the features
    *****************************************************************************/
   vector< vector<double> > getFeatures()
   {
      return mFeatures;
   }
   
   /******************************************************************************
    * Return the number of feature arrays
    *****************************************************************************/
   int featureSize()
   {
      return mFeatures.size();
   }
   
   /******************************************************************************
    * Change the features to 'features'
    *****************************************************************************/
   void setFeatures(vector< vector<double> > features)
   {
      mFeatures = features;
   }

private:
   string mSpeakerName;
   vector< vector<double> > mFeatures;
};

#endif

