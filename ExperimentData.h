
#ifndef __EXPERIMENT_DATA__
#define __EXPERIMENT_DATA__

#include <string>
#include <fstream>
#include <iostream>
#include <time.h>

class EXPERIMENT2TRIAL{
	public:
		//Default Constructor//
		EXPERIMENT2TRIAL() : image(0), starting_percent(0.1f)
		{	}

		//Parameter Constructor//
		EXPERIMENT2TRIAL(int img, float sperc)
		{ image = img; starting_percent = sperc; }

		//Default Destructor//
		~EXPERIMENT2TRIAL()
		{	}
	
		//Data Members//
		int image;
		float starting_percent;

	private:
};

class EXPERIMENT2DATA{
	public:
		//Default Constructor..
		EXPERIMENT2DATA() : image(0), pupil_diam(0.0f), user_sigma(0.0f), real_sigma(0.0f), seconds(0.0)
		{	}

		//Parameter Constructor//
		EXPERIMENT2DATA(int img, float pdiam, float usigma, float rsigma, double sec)
		{ image = img; pupil_diam = pdiam; user_sigma = usigma; real_sigma = rsigma; seconds = sec; }

		//Default Destructor//
		~EXPERIMENT2DATA()
		{	}

		//Data Members//
		int image;
		float pupil_diam;
		float user_sigma;
		float real_sigma;
		double seconds;

	private:
};

class EXPERIMENT3TRIAL{
	public:
		//Default Constructor//
		EXPERIMENT3TRIAL() : image(0), real_image(0), use_sigma(0)
		{	}

		//Parameter Constructor, image, real image, use sigma//
		EXPERIMENT3TRIAL(int img, int rimg, int usgma)
		{ image = img; real_image = rimg; use_sigma = usgma; }

		//Default Destructor//
		~EXPERIMENT3TRIAL()
		{	}

		//Data Members//
		int image;
		int real_image;
		int use_sigma;

	private:
};

class EXPERIMENT3DATA{
	public:
		//Default Constructor//
		EXPERIMENT3DATA() : image(0), real_image(0), pupil_diam(0.0f), sigma(0.0), match(0), use_sigma(0), seconds(0.0)
		{	}

		//Parameter Constructor//
		EXPERIMENT3DATA( int img, int rimg, float pdiam, float sig, int m, int usgma, double sec )
		{ image = img; real_image = rimg; pupil_diam = pdiam; sigma = sig; match = m; use_sigma = usgma, seconds = sec; }

		//Default Destructor//
		~EXPERIMENT3DATA()
		{	}

		//Data Members//
		int image;
		int real_image;
		float pupil_diam;
		float sigma;
		int match;
		int use_sigma;
		double seconds;

	private:
};


#endif	//__EXPERIMENT_DATA__