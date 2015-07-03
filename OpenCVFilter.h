
#ifndef __OPENCV_FILTER__
#define __OPENCV_FILTER__

//This program makes sharpened image according to PSF(Point Spread Function)

#include <opencv2/opencv.hpp>

#include <iostream>
#include <fstream>
#include <strstream>

using namespace cv;
using namespace std;

#define PIXEL_SCALE 2.58
#define SCREEN_DISTANCE 700

class OPENCVIMGFILTER
{
	public:
		OPENCVIMGFILTER() : sn_inverse(0.050), 	width_of_blur(10.0), width(0), height(0)
		{	}

		~OPENCVIMGFILTER()
		{	}

		void LoadImage(string img)
		{ src = cv::imread(img, 0);	img_src = img; width = src.cols; height = src.rows; }
		
		float UpdateSigma(float pupil_size, float real_distance)
		{
			width_of_blur = pupil_size/2.0f*abs(1.0f - SCREEN_DISTANCE/real_distance)/PIXEL_SCALE;
			return width_of_blur;
		}

		void DeconvolveImg(float percent_of_sigma = 1.0)
		{
			//Calculating eye's PSF
			Mat_<double> gaussian_real = Gaussian_picture(width_of_blur*percent_of_sigma, width, height);

			//Deconvolution according to PSF
			deconvolution_result = Wiener_Filter(src, gaussian_real, sn_inverse, width, height);

			deconvolution_result.convertTo(deconvolution_result, CV_8U);
		}

		void SaveDeconvResult()
		{ cv::imwrite("deconv_" + img_src, deconvolution_result); }

		cv::Mat src;
		cv::Mat deconvolution_result;

		double width_of_blur;	//Blur radius(pixel)
		double sn_inverse;	//Inverse of signal to noise ratio
		int width;	// width of image 
		int height;	// height of image

	private:
		string img_src;

		//Replacement 4 quadrant
		cv::Mat ShiftDFT(cv::Mat src_arr, int IMAGE_WIDTH, int IMAGE_HEIGHT)
		{
			int cx = IMAGE_WIDTH/2, cy = IMAGE_HEIGHT/2;
			int i, j;
			cv::Mat dst_arr = cv::Mat::zeros(IMAGE_HEIGHT, IMAGE_WIDTH, CV_64F);

			for(j=0;j<cy;j++){
				for(i=0;i<cx;i++){
		
					dst_arr.at<double>(j,i) 	=	src_arr.at<double>(j + cy,i + cx);	
					dst_arr.at<double>(j + cy,i) 	=	src_arr.at<double>(j,i + cx);
					dst_arr.at<double>(j,i + cx) 	=	src_arr.at<double>(j + cy,i);
					dst_arr.at<double>(j + cy,i + cx) 	=	src_arr.at<double>(j,i);
		
				}
			}

			return dst_arr;
		}

		//Merging for Fourier Transform
		cv::Mat Merge(cv::Mat_<double> src_arr, cv::Mat_<double> src_arr2)
		{
			std::vector<cv::Mat> mv;
			mv.push_back(src_arr);
			mv.push_back(src_arr2);
  
			cv::Mat complex;
			cv::merge(mv, complex);

			return complex;
		}

		//Splitting for Fourier Transform
		cv::Mat Split(cv::Mat src_arr, int flag)
		{
			std::vector<cv::Mat> complex;
			cv::split(src_arr, complex);

			if(flag == 0){

				return complex[0];

			}else{
	
				return complex[1];

			}

		}

		//Generating gaussian function
		cv::Mat Gaussian_picture(double SIGMA, int IMAGE_WIDTH, int IMAGE_HEIGHT)
		{
			int cx = IMAGE_WIDTH/2, cy = IMAGE_HEIGHT/2;
			int i, j;
			double SIGMA_variable = 1 / (2*SIGMA*SIGMA);
			double distance;

			cv::Mat dst_arr = cv::Mat::zeros(IMAGE_HEIGHT, IMAGE_WIDTH, CV_64F);

			for(j=0;j<IMAGE_HEIGHT;j++){
				for(i=0;i<IMAGE_WIDTH;i++){
		
					distance = double(cx - i) * double(cx - i) + double(cy - j) * double(cy - j);
					dst_arr.at<double>(j,i) = (SIGMA_variable / CV_PI) * exp(-distance * SIGMA_variable);		
		
				}
			}

			return dst_arr;
		}

		//Wiener Filter
		cv::Mat Wiener_Filter(cv::Mat_<double> src_arr, cv::Mat_<double> src_arr2,double SN_inverse, int IMAGE_WIDTH, int IMAGE_HEIGHT)
		{
			cv::Mat defocusimg_dft, gaussian_dft;
			cv::Mat mix_real, mix_imaginary;
			cv::Mat mix_real2, mix_imaginary2;

			//Initialize
			cv::Mat_<double> defocusimg_imaginary = cv::Mat::zeros(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8U);

			//Generating 2-channel array for the real and imaginary parts
			cv::Mat defocusimg_complex = Merge(src_arr, defocusimg_imaginary);
			cv::Mat gaussian_complex = Merge(src_arr2, defocusimg_imaginary);

	
			//Fourier Transform
			cv::dft(defocusimg_complex, defocusimg_dft);
			cv::dft(gaussian_complex, gaussian_dft);


			//Dividinfg the Fourier transform to the real and imaginary parts
			cv::Mat defocusimg_dft_real = Split(defocusimg_dft, 0);
			cv::Mat defocusimg_dft_imaginary = Split(defocusimg_dft, 1);

			cv::Mat gaussian_dft_real = Split(gaussian_dft, 0);
			cv::Mat gaussian_dft_imaginary = Split(gaussian_dft, 1);


			//Gaussian power
			cv::Mat gaussian_spectrum = gaussian_dft_real.mul(gaussian_dft_real) + gaussian_dft_imaginary.mul(gaussian_dft_imaginary);
			cv::Mat gaussian_sqrt = gaussian_spectrum + SN_inverse;

	
			//Wiener filter
			mix_real2 = defocusimg_dft_real.mul(gaussian_dft_real) + defocusimg_dft_imaginary.mul(gaussian_dft_imaginary);
			mix_imaginary2 = defocusimg_dft_imaginary.mul(gaussian_dft_real) - defocusimg_dft_real.mul(gaussian_dft_imaginary);

			mix_real = mix_real2 / gaussian_sqrt;
			mix_imaginary = mix_imaginary2 / gaussian_sqrt;	


			//Merging of real and imaginary
			cv::Mat Gaussian_deconvolution = Merge(mix_real, mix_imaginary);


			//Inverse Fourier Transform
			cv::Mat defocusimg_idft;
			cv::idft(Gaussian_deconvolution, defocusimg_idft,DFT_SCALE);


			//Picking up the real part
			cv::Mat idft_result = Split(defocusimg_idft, 0);


			//Replacement 4 quadrant
			cv::Mat idft_result_shift = ShiftDFT(idft_result, IMAGE_WIDTH, IMAGE_HEIGHT);


			return idft_result_shift;
		}

};


#endif //__OPENCV_FILTER__//