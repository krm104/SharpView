/****************************************************************************
This is the main Interface for the SharpView User Experiment

Conducted during June 2015
****************************************************************************/

//Standard Includes//
#include <string.h>
#include <fstream>

//Projec Includes//
#include "PupilZMQ.h"
#include "CalibrateUtils.h"
#include "OpenCVFilter.h"
#include "ExperimentData.h"

//OpenGL related Includes//
#define GLEW_STATIC 1
#include <GLSL\glModel.h>

#include <GL/glui.h>

#define GLFW_NO_GLU
#include <gl\glfw3.h>

//Image Files//
vector<OPENCVIMGFILTER> images;
vector<OPENCVIMGFILTER> gauge_images;

//Square for Image Display//
gl_Shader shader_image;	//Shader for rendering the image texture
GLuint image_texture = 0;	//buffer handle for the image texture

//Pupil Communication Object//
PUPILZMQ pupil_comm;	//Object for controlling communication with Pupil software
#define IRIS_SIZE 12.0	//the average iris size for people
#define PIXEL_SCALE 2.58	//approximate size of each pixel on the display screen when seen at distance
#define SCREEN_DISTANCE 700	//focal distance of display screen
#define SQUARE_SIZE 0.27	//normalized half size of the image for display on the screen 

//Calibration Crosshair vertices//
CROSSHAIR crosshair;	//Object for representing the currently displayed calibration crosshair//
pair<float, float> crosshairlocations[10];	//list of all calibration crosshair locations

//Global Variables//
double aspect = 0.0;	//aspect ratio of the rendering window
float WIDTH = 0;	//width of the rendering window
float HEIGHT = 0;	//height of the rendering window
bool joybuttons[14];	//array representing the button states for the controller

string experiment2_file = "";	//name of the experiment 2 trial file loaded
string experiment2_data_file = "";	//name of the experiment 2 data file to be written
vector<EXPERIMENT2TRIAL> experiment2_trials;	//list of all the experiment 2 trials for the current subject and distance
vector<EXPERIMENT2DATA>	experiment2_data;	//list of all the experiment 2 data for each of the current trials

string experiment3_file = "";	//name of the experiment 3 trial file loaded
string experiment3_data_file = "";	//name of the experiment 3 data file to be written
vector<EXPERIMENT3TRIAL> experiment3_trials;	//list of all the experiment 3 trials for the current subject and distance
vector<EXPERIMENT3DATA>	experiment3_data;	//list of all the experiment 3 data for each of the current trials

time_t trial_start_time;	//the current time (recorded once a trial image is shown

vector<clock_t> gaze_check;	//list of the last n number of gaze points directed at the on screen image
bool rv = false;	//return value for checkGaze method//

//Glut Window Handles//
int main_window = 0;	//handle to the main rendering window

//Glui Live Variables//
int calibrate_b = 0;	//value of calibration state
int experiment2_b = 0;	//value of experiment2 state
int experiment3_b = 0;	//value of experiment3 state

float calibrate_x = 0.275;	//horizontal calibration adjustment
float calibrate_y = 0.525;	//vertical calibration adjustment
int calibrate_number = 0;	//number of verification point on screen

int render_verify = 0;	//show verification rendering in calibration window
int user_action = 0;	//user selection activation

int use_gaze_check = 0;	//use the gaze tracking to turn off the CG window if the user is looking at it for too long
int gaze_correction_x = 0;	//offset value used to adjust the gaze values passed in by Pupil Software
int gaze_correction_y = 0;	//offset value used to adjust the gaze values passed in by Pupil Software

int experiment2_subject = -1;	//subject number for experiment2
int experiment2_distance = -1;	//distance to real image for experiment2
int experiment2_trial = 0;	//current trial number of experiment2
int experiment2_iris = 0;	//iris size in pixels
float experiment2_user_sigma = 0;	//user selected sigma value
float experiment2_sigma = 0;	//psf calculated sigma
int experiment2_show = 0;	//show image
int experiment2_image= 0;	//current image displayed for experiment2
float experiment2_pupil = 0.0f;	//pupil size in mm

int experiment3_subject = -1;	//subject number for experiment3
int experiment3_distance = -1;	//distance to real image for experiment3
int experiment3_trial = 0;	//current trial number of experiment3
int experiment3_iris = 0;	//iris size in pixels
float experiment3_sigma = 0;	//psf calculated sigma
int experiment3_show = 0;	//show onscreen image
int experiment3_image = 0;	//current image displayed onscreen for experiment3
int experiment3_real_image = 0;	//current real image shown for experimen3
float experiment3_pupil = 0.0f;	//pupil size in mm
int experiment3_match = 0;	//value denoting whether the real and onscreen images match
int experiment3_use_sigma = 0;	//value denoting whether the onscreen image will use sigma or not

//Glui Control Pointers//
GLUI* glui = NULL;	//Glui control pane//

GLUI_Panel* experiment_panel = NULL;	//Panel with experiment state buttons//
GLUI_Panel* calibration_panel = NULL;	//Panel with calibration specific controls//
GLUI_Panel* experiment2_panel = NULL;	//Panel with experiment 2 specific controls//
GLUI_Panel* experiment2_subject_panel = NULL;	//panel with experiment 2 subject/trial information//
GLUI_Panel* experiment2_trial_panel = NULL;	//panel with experiment 2 trial/image information//
GLUI_Panel* experiment2_eye_panel = NULL;	//panel with experiemnt 2 eye specific information//
GLUI_Panel* experiment2_sigma_panel = NULL;	//panel with experiment 2 sigma information//
GLUI_Panel* experiment2_show_save_panel = NULL;	//panel with experiment 2 data control buttons//
GLUI_Panel* experiment3_panel = NULL;	//Panel with experiment 3 specific controls//
GLUI_Panel* experiment3_subject_panel = NULL;	//panel with experiment 3 subject/trial information//
GLUI_Panel* experiment3_trial_panel = NULL;	//panel with experiment 3 trial/image information//
GLUI_Panel* experiment3_eye_panel = NULL;	//panel with experiemnt 3 eye specific information//
GLUI_Panel* experiment3_image_panel = NULL;	//panel with experiment 3 image information//
GLUI_Panel* experiment3_show_save_panel = NULL;	//panel with experiment 3 data control buttons//

GLUI_Checkbox* use_gaze_check_checkbox = NULL;	//checkbox for denoting if the gaze check for hiding the onscreen images should be used
GLUI_Checkbox* user_action_checkbox = NULL;	//checkbox denoting that the user has made an action during calibration
GLUI_Checkbox* render_verification_checkbox = NULL;	//checkbox denoting that the verification square should be shown during calibration
GLUI_Checkbox* calibrate_checkbox = NULL;	//checbox denoting the calibration is the current active state
GLUI_Checkbox* experiment2_checkbox = NULL;	//checkbox denoting that experiment 2 is the current active state
GLUI_Checkbox* experiment2_show_checkbox = NULL;	//checkbox denoting that the current trial's image should be shown
GLUI_Checkbox* experiment3_checkbox = NULL;	//checkbox denoting that experiment 3 is the current active state
GLUI_Checkbox* experiment3_show_checkbox = NULL;	//checbox denoting that the current trial's image should be shown

GLUI_Button* calibrate_button = NULL;	//button used for activating the calibtration state
GLUI_Button* experiment2_button = NULL;	//button used for activating the experiment 2 state
GLUI_Button* experiment2_save_button = NULL;	//button used for writing the current experiment 2 data to a file
GLUI_Button* experiment3_button = NULL;	//button used for activating the experiment 3 state
GLUI_Button* experiment3_save_button = NULL;	//button used for writing the current experiment 3 data to a file

GLUI_Spinner* calibrate_x_spinner = NULL;	//horizontal offset value for the on screen image
GLUI_Spinner* calibrate_y_spinner = NULL;	//vertical offset value for the on screen image
GLUI_Spinner* gaze_correction_x_spinner = NULL;	//horizontal correction value for user gaze from Pupil software
GLUI_Spinner* gaze_correction_y_spinner = NULL;	//vertical correction value for the user gaze from Pupil software
GLUI_Spinner* calibrate_number_spinner = NULL;	//spinner listing the currently active calibration cross
GLUI_Spinner* experiment2_subject_spinner = NULL;	//spinner listing the subject ID for the current experiment 2 trial set
GLUI_Spinner* experiment2_distance_spinner = NULL;	//spinner listing the real image distance for the current experiment 3 trial set
GLUI_Spinner* experiment2_trial_spinner = NULL;	//spinner listing the current trial for experiment 2
GLUI_Spinner* experiment2_image_spinner = NULL;	//spinner listing the current on-screen image for the active experiment 2 trial
GLUI_Spinner* experiment2_iris_spinner = NULL;	//spinner listing the IRIS size in ppixels for the experiment 2 pupil size calculations
GLUI_Spinner* experiment2_pupil_spinner = NULL;	//spinner listing the pupil size in mm for the experiment 2 sigma calculations
GLUI_Spinner* experiment2_sigma_spinner = NULL;	//spinner listing the calculated sigma value for the current experiment 2 trial
GLUI_Spinner* experiment2_user_sigma_spinner = NULL;	//spinner listing the user selected sigma value for the current experiment 2 trial
GLUI_Spinner* experiment3_subject_spinner = NULL;	//spinner listing the subject ID for the current experiment 3 trial set
GLUI_Spinner* experiment3_distance_spinner = NULL;	//spinner listing the real image distance for the current experiment 3 trial set
GLUI_Spinner* experiment3_trial_spinner = NULL;	//spinner listing the current trial for experiment 3
GLUI_Spinner* experiment3_image_spinner = NULL;	//spinner listing the current on-screen image for the active experiment 3 trial
GLUI_Spinner* experiment3_real_image_spinner = NULL;	//spinner listing the currently seen real image for the active experiment 3 trial
GLUI_Spinner* experiment3_iris_spinner = NULL;	//spinner listing the IRIS size in pixels for the experiment 3 pupil size calculations
GLUI_Spinner* experiment3_pupil_spinner = NULL;	//spinner listing the pupil size in mm for the expeirment 3 sigma calculations
GLUI_Spinner* experiment3_sigma_spinner = NULL;	//spinner listing the calculated sigma value for the current experiment 3 trial
GLUI_Spinner* experiment3_use_sigma_spinner = NULL;	//spinner listing the boolean flag for use of the experiment 3 sigma value for the trial

GLUI_FileBrowser* experiment2_file_browser = NULL;	//file browser for selecting the experiment 2 trial sets
GLUI_FileBrowser* experiment3_file_browser = NULL;	//file browser for selecting the experiment 3 trial sets

//Glui Control ID's//
enum control_id
{
	RENDER_VERIFICATION_CHECKBOX,
	USER_ACTION_CHECKBOX,
	CALIBRATE_CHECKBOX,
	EXPERIMENT2_CHECKBOX,
	EXPERIMENT2_SHOW_CHECKBOX,
	EXPERIMENT3_CHECKBOX,
	EXPERIMENT3_SHOW_CHECKBOX,

	CALIBRATE_BUTTON,
	EXPERIMENT2_BUTTON,
	EXPERIMENT2_SAVE_BUTTON,
	EXPERIMENT3_BUTTON,
	EXPERIMENT3_SAVE_BUTTON,

	CALIBRATE_X_SPINNER,
	CALIBRATE_Y_SPINNER,
	CALIBRATE_NUMBER_SPINNER,
	EXPERIMENT2_TRIAL_SPINNER,
	EXPERIMENT2_USER_SIGMA_SPINNER,
	EXPERIMENT3_TRIAL_SPINNER,

	EXPERIMENT2_FILE_BROWSER,
	EXPERIMENT3_FILE_BROWSER,
};

/**********************************************************************
Build set of crosshairs to show during eye tracker calibration.
**********************************************************************/
void BuildCrossHairLocations()
{
	cout << "Setting up Calibration Crosshairs..." << endl;
	//Set of Calibration Crosshair locations//
	crosshairlocations[0] = pair<float, float>(-1.0f, -1.0f);
	crosshairlocations[1] = pair<float, float>(0.1666f, 0.1666f);
	crosshairlocations[2] = pair<float, float>(0.1666f, 0.8333f);
	crosshairlocations[3] = pair<float, float>(0.8333f, 0.8333f);
	crosshairlocations[4] = pair<float, float>(0.8333f, 0.1666);
	crosshairlocations[5] = pair<float, float>(0.3333f, 0.6666f);
	crosshairlocations[6] = pair<float, float>(0.3333f, 0.3333f);
	crosshairlocations[7] = pair<float, float>(0.6666f, 0.6666f);
	crosshairlocations[8] = pair<float, float>(0.6666f, 0.3333f);
	crosshairlocations[9] = pair<float, float>(0.5f, 0.5f);
}

/**********************************************************************
Load Images used in the experiment into OPENCVIMGFILTER objects,
perform a SharpView deconvolution to set the deconvolve image object.
**********************************************************************/
void LoadImages()
{
	cout << "Creating Image Objects..." << endl;
	//Create Image Objects for the 8 images//
	for ( int i = 0; i <= 12; i++ )
	{
		images.push_back(OPENCVIMGFILTER());
	}

	for ( int i = 0; i < 4; i++ )
	{
		gauge_images.push_back(OPENCVIMGFILTER());
	}

	cout << "Loading Image Objects..." << endl;
	//Load the 8 Images and perform default convolve//
	images[0].LoadImage("Stone_0.jpg");
	images[0].DeconvolveImg();
	images[1].LoadImage("Stone_1.jpg");
	images[1].DeconvolveImg();
	images[2].LoadImage("Stone_2.jpg");
	images[2].DeconvolveImg();
	images[3].LoadImage("Stone_3.jpg");
	images[3].DeconvolveImg();
	images[4].LoadImage("Stone_4.jpg");
	images[4].DeconvolveImg();
	images[5].LoadImage("Stone_5.jpg");
	images[5].DeconvolveImg();
	images[6].LoadImage("Stone_6.jpg");
	images[6].DeconvolveImg();
	images[7].LoadImage("Stone_7.jpg");
	images[7].DeconvolveImg();
	images[8].LoadImage("Stone_8.jpg");
	images[8].DeconvolveImg();
	images[9].LoadImage("Stone_9.jpg");
	images[9].DeconvolveImg();
	images[10].LoadImage("Stone_10.jpg");
	images[10].DeconvolveImg();
	images[11].LoadImage("Stone_11.jpg");
	images[11].DeconvolveImg();
	images[12].LoadImage("Stone_12.jpg");
	images[12].DeconvolveImg();

	//Load the 4 gauge images//
	gauge_images[0].LoadImage("gauge_1.jpg");
	gauge_images[1].LoadImage("gauge_2.jpg");
	gauge_images[2].LoadImage("gauge_3.jpg");
	gauge_images[3].LoadImage("gauge_4.jpg");

	cout << "Images Loaded..." << endl;
}

/**********************************************************************
Calculate and return the pupil size of the subject in mm.
The IRIS_SIZE value is fixed (12mm) while the diameter in pixels of the iris
can be set by the experiment controls. The pupil diameter in pixels is
determined by the Pupil Labs software. The mm/pixel value is calculated
using the iris values and the final pupil size in mm then found using
the Pupil Labs pupil pixel value.
**********************************************************************/
void getPupilSize()
{
	if ( pupil_comm.pdiam > 0.0f )
	{
		experiment2_pupil = IRIS_SIZE/experiment2_iris*pupil_comm.pdiam;
		experiment3_pupil = IRIS_SIZE/experiment3_iris*pupil_comm.pdiam;
	}
	
	//double subpupil = 80.0;
	//experiment2_pupil = IRIS_SIZE/experiment2_iris*subpupil;
	//experiment3_pupil = IRIS_SIZE/experiment3_iris*subpupil;
}

/**********************************************************************
Function to process Pupil Software Data and update pupil size and gaze
**********************************************************************/
void processEyeMessages()
{
	//Process Pupil Software Data//
	pupil_comm.processMessage();
	//Update Pupil Size Information//
	getPupilSize();
}

/**********************************************************************
Function to check if the gaze has been too long on the CG screen
**********************************************************************/
bool checkGaze()
{
	//check if subject is looking into the CG area//
	if ( (pupil_comm.normx + (float)gaze_correction_x/100.0f) - .005 <= (calibrate_x + SQUARE_SIZE/2.0) )
	{
	rv = true;
	}else
		rv = false;
		/*
		//get current time//
		clock_t now = pupil_comm.mtime;
		//cout << now << endl;
		if ( gaze_check.size() )
		{
			cout << now - gaze_check[0] << endl;
			if ( now == gaze_check.back() )
			{
				return rv;
			} else
			{
				//rv = false;
			}

			//check if the time between the last 5 gazes into the area has been within the last few seconds//
			if ( gaze_check.size() > 3 && (now - gaze_check[0]) > 300 )//difftime(now, gaze_check[0]) <= 4 )
			{
				rv = true;
			}
		}

		gaze_check.push_back(now);
		
		if ( gaze_check.size() > 5 )
		{
			//remove the oldest value//
			gaze_check.erase(gaze_check.begin(),gaze_check.begin()+3 );
		}
	}else
	{
		rv = false;
	}
	*/

	return rv;
}

/**********************************************************************
Function to process Pupil Software Data and update pupil size and gaze
**********************************************************************/
float CalcSigma( float pupil_size, float real_distance )
{
	return pupil_size/2.0f*abs(1.0f - SCREEN_DISTANCE/real_distance)/PIXEL_SCALE;
}

/**********************************************************************
Function to reset the experiment 2 state before loading a new file
**********************************************************************/
void resetExperiment2()
{
	experiment2_subject = -1;	//subject number for experiment2
	experiment2_distance = -1;	//distance to real image for experiment2
	experiment2_trial = 0;	//current trial number of experiment2
	experiment2_user_sigma = 0;	//user selected sigma value
	experiment2_sigma = 0;	//psf calculated sigma
	experiment2_show = 0;	//show image
	experiment2_image= 0;	//current image displayed for experiment2
	experiment2_pupil = 0.0f;	//pupil size in mm

	experiment2_file = "";
	experiment2_data_file = "";
	experiment2_trials.clear();
	experiment2_data.clear();
}

/**********************************************************************
Function to load and read data from Experiment 2 file.
**********************************************************************/
bool readExperiment2Data(string file)
{ 
	//clear current trial set//
	resetExperiment2();
	experiment2_trial_spinner->set_int_limits(0, 0);

	//verify that the file selected is of the proper type//
	size_t extension_pos = file.rfind(".ex2");
	if ( (extension_pos != string::npos)  && (extension_pos == file.length() - 4)){
		//Open the file for reading//
		ifstream experiment2_file(file);
		if (experiment2_file.good()){
			//Read in the trial data//
			string token;
			while ( !experiment2_file.eof() ){
				experiment2_file >> token;
				if ( token == "sub:" )
					experiment2_file >> experiment2_subject;
				if ( token == "dis:" )
					experiment2_file >> experiment2_distance;
				if ( token == "img:" ){
					int img; float sperc;
					experiment2_file >> img;
					experiment2_file >> sperc;
					experiment2_trials.push_back(EXPERIMENT2TRIAL(img, sperc));
					experiment2_data.push_back(EXPERIMENT2DATA());
				}
			}
			//close the file and prepare the output file name//
			experiment2_file.close();
			experiment2_file.clear();
			experiment2_data_file = file + "_";
			experiment2_data_file.insert(extension_pos, "_data");
			return true;
		}
		experiment2_file.close();
		experiment2_file.clear();
	}
	//file was not of the proper type//
	experiment2_subject = -1;
	experiment2_distance = -1;

	return false;
}

/**********************************************************************
Function to save the data for the current Experiment 2 trial.
**********************************************************************/
void saveExperiment2Data()
{
	//check if a proper data file name is available//
	if ( experiment2_data_file != "" )
	{
		//open output data file for writing//
		ofstream saveFile(experiment2_data_file);
		//Write data formate header//
		saveFile << "sub:\t" << experiment2_subject << "\tdis:\t" << experiment2_distance << endl;
		saveFile << "trial\timage\tpupil_diam\tuser_sigma\treal_sigma\ttime" << endl;
		for ( unsigned int i = 0; i < experiment2_data.size(); i++ )
		{
			saveFile << i << "\t" << experiment2_data[i].image << "\t" << experiment2_data[i].pupil_diam << "\t" << experiment2_data[i].user_sigma << "\t" << experiment2_data[i].real_sigma << "\t" << experiment2_data[i].seconds << endl;
		}
		saveFile.close();
		saveFile.clear();
	}
}

/**********************************************************************
Function to set the state of controls and values for the current
experiment 2 trial. The proper image for display is rendered and
the pupil size and sigma values updated for the user.
**********************************************************************/
void setExperiment2State(int trial_num)
{
	if ( (int)experiment2_trials.size() - 1 == trial_num )
	{
		saveExperiment2Data();
	}

	//check for valid trial number//
	if ( (int)experiment2_trials.size() - 1 >= trial_num )
	{
		//set current image//
		experiment2_image = experiment2_trials[trial_num].image;

		//set starting user_sigma percent//
		//experiment2_user_sigma_spinner->set_float_val(experiment2_trials[trial_num].starting_percent);

		//update user pupil and real sigma value//
		processEyeMessages();
		experiment2_sigma = CalcSigma(experiment2_pupil, experiment2_distance);//images[experiment2_image].UpdateSigma(experiment2_pupil, experiment2_distance);

		experiment2_user_sigma = images[experiment2_image].width_of_blur = experiment2_trials[trial_num].starting_percent;
		images[experiment2_image].DeconvolveImg();//experiment2_user_sigma);
	}
}

/**********************************************************************
Function to save the state of controls and values for the current
experiment 2 trial. 
**********************************************************************/
void saveExperiment2State(int trial_num)
{
	//check for valid trial number//
	if ( (int)experiment2_trials.size() - 1 >= trial_num )
	{
		//get trial time//
		time_t trial_end_time;
		time(&trial_end_time);

		experiment2_data[trial_num] = EXPERIMENT2DATA(experiment2_image, experiment2_pupil, experiment2_user_sigma, experiment2_sigma, difftime(trial_end_time, trial_start_time)); 
	}
}

/**********************************************************************
Function to reset the experiment 3 state before loading a new file
**********************************************************************/
void resetExperiment3()
{
	experiment3_subject = -1;	//subject number for experiment3
	experiment3_distance = -1;	//distance to real image for experiment3
	experiment3_trial = 0;	//current trial number of experiment3
	experiment3_sigma = 0;	//psf calculated sigma
	experiment3_show = 0;	//show onscreen image
	experiment3_image = 0;	//current image displayed onscreen for experiment3
	experiment3_real_image = 0;	//current real image shown for experimen3
	experiment3_pupil = 0.0f;	//pupil size in mm
	experiment3_match = 0;	//value denoting whether the real and onscreen images match
	experiment3_use_sigma = 0;	//value denoting whether the onscreen image will use sigma or not

	experiment3_file = "";
	experiment3_data_file = "";
	experiment3_trials.clear();
	experiment3_data.clear();
}

/**********************************************************************
Function to load and read data from Experiment 3 file.
**********************************************************************/
bool readExperiment3Data(string file)
{ 
	//clear current trial set//
	resetExperiment3();
	experiment3_trial_spinner->set_int_limits(0, 0);

	//verify that the file selected is of the proper type//
	size_t extension_pos = file.rfind(".ex3");
	if ( (extension_pos != string::npos)  && (extension_pos == file.length() - 4)){
		//Open the file for reading//
		ifstream experiment3_file(file);
		if (experiment3_file.good()){
			//Read in the trial data//
			string token;
			while ( !experiment3_file.eof() ){
				experiment3_file >> token;
				if ( token == "sub:" )
					experiment3_file >> experiment3_subject;
				if ( token == "dis:" )
					experiment3_file >> experiment3_distance;
				if ( token == "img:" ){
					int img; int rimg; int usgma;
					experiment3_file >> img;
					experiment3_file >> rimg;
					experiment3_file >> usgma;
					experiment3_trials.push_back(EXPERIMENT3TRIAL(img, rimg, usgma));
					experiment3_data.push_back(EXPERIMENT3DATA());
				}
			}
			//close the file and prepare the output file name//
			experiment3_file.close();
			experiment3_file.clear();
			experiment3_data_file = file + "_";
			experiment3_data_file.insert(extension_pos, "_data");
			return true;
		}
		experiment3_file.close();
		experiment3_file.clear();
	}
	//file was not of the proper type//
	experiment3_subject = -1;
	experiment3_distance = -1;

	return false;
}

/**********************************************************************
Function to save the data for the current Experiment 3 trial.
**********************************************************************/
void saveExperiment3Data()
{
	//check if a proper data file name is available//
	if ( experiment3_data_file != "" )
	{
		//open output data file for writing//
		ofstream saveFile(experiment3_data_file);
		//Write data formate header//
		saveFile << "sub:\t" << experiment3_subject << "\tdis:\t" << experiment3_distance << endl;
		saveFile << "trial\timage\treal_image\tmatch\tcorr_match\tpupil_diam\tsigma\ttime" << endl;
		for ( unsigned int i = 0; i < experiment3_data.size(); i++ )
		{
			saveFile << i << "\t" << experiment3_data[i].image << "\t" << experiment3_data[i].real_image << "\t" << experiment3_data[i].match << "\t";
			if ( experiment3_data[i].image == experiment3_data[i].real_image )
				saveFile << "1\t";
			else
				saveFile << "0\t";

			saveFile << experiment3_data[i].pupil_diam << "\t" << experiment3_data[i].sigma*experiment3_data[i].use_sigma << "\t" << experiment3_data[i].seconds << endl;
		}
		saveFile.close();
		saveFile.clear();
	}
}

/**********************************************************************
Function to set the state of controls and values for the current
experiment 3 trial. The proper image for display is rendered and
the pupil size and sigma values updated for the user.
**********************************************************************/
void setExperiment3State(int trial_num)
{
	if ( (int)experiment3_trials.size() - 1 == trial_num )
	{
		saveExperiment3Data();
	}

	//check for valid trial number//
	if ( (int)experiment3_trials.size() - 1 >= trial_num )
	{
		//set current image//
		experiment3_image = experiment3_trials[trial_num].image;
		experiment3_real_image = experiment3_trials[trial_num].real_image;
		experiment3_use_sigma = experiment3_trials[trial_num].use_sigma;

		//update user pupil and real sigma value//
		processEyeMessages();
		experiment3_sigma = images[experiment3_image].UpdateSigma(experiment3_pupil, experiment3_distance);
		images[experiment3_image].DeconvolveImg();
	}
}

/**********************************************************************
Function to save the state of controls and values for the current
experiment 3 trial.
**********************************************************************/
void saveExperiment3State(int trial_num)
{
	//check for valid trial number//
	if ( (int)experiment3_trials.size() - 1 >= trial_num )
	{
		//get trial time//
		time_t trial_end_time;
		time(&trial_end_time);
		
		//save trial data values//
		experiment3_data[trial_num] = EXPERIMENT3DATA(experiment3_image, experiment3_real_image, experiment3_pupil, experiment3_sigma, experiment3_match, experiment3_use_sigma, difftime(trial_end_time, trial_start_time)); 
	}
}

/**********************************************************************
Function to process actions made using the GLUI interface controls.
These controls are only activated by the experimenter. Each control
uses a unique identifier which is passed into the control parameter
by the calling control. A switch block is then used to determine
the calling control.
**********************************************************************/
void control_cb( int control )
{
	switch (control )
	{
		case USER_ACTION_CHECKBOX:	
			break;

		case CALIBRATE_BUTTON:
			calibrate_b = 1; experiment2_b = 0; experiment3_b = 0;
			calibration_panel->enable(); experiment2_panel->disable(); experiment3_panel->disable();
			user_action_checkbox->disable(); calibrate_x_spinner->disable(); calibrate_y_spinner->disable();
			break;

		case CALIBRATE_NUMBER_SPINNER:
			user_action = 0;
			break;

		case EXPERIMENT2_BUTTON:
			calibrate_b = 0; experiment2_b = 1; experiment3_b = 0;
			calibration_panel->disable(); experiment2_panel->enable(); experiment3_panel->disable();
			gaze_correction_x_spinner->enable(); gaze_correction_y_spinner->enable();
			experiment2_subject_panel->disable(); experiment2_image_spinner->disable(); experiment2_pupil_spinner->disable();
			//experiment2_user_sigma_spinner->disable(); experiment2_sigma_spinner->disable();
			if ( experiment2_data_file.length() <= 1 ) experiment2_save_button->disable();
			break;

		case EXPERIMENT2_FILE_BROWSER:
			experiment2_show = 0;
			experiment2_file = "";
			experiment2_file = experiment2_file_browser->get_file();
			if ( readExperiment2Data(experiment2_file) ) {
				experiment2_save_button->enable();
				experiment2_trial_spinner->set_int_limits(0, experiment2_trials.size() - 1);
				experiment2_trial = 0;
				//experiment2_trial_spinner->set_int_val(experiment2_trial);
				setExperiment2State(experiment2_trial);
			}else {
				experiment2_save_button->disable();
			}
			break;

		case EXPERIMENT2_TRIAL_SPINNER:
			//experiment2_show = 0;
			//if ( experiment2_trial_spinner->last_int_val < experiment2_trial_spinner->int_val )
			{
				saveExperiment2State(experiment2_trial_spinner->last_int_val);
				setExperiment2State(experiment2_trial);
			}
			break;

		case EXPERIMENT2_USER_SIGMA_SPINNER:
			images[experiment2_image].width_of_blur = experiment2_user_sigma;
				images[experiment2_image].DeconvolveImg();
			break;

		case EXPERIMENT2_SAVE_BUTTON:
			saveExperiment2Data();
			break;

		case EXPERIMENT2_SHOW_CHECKBOX:
			if ( experiment2_show )
				time(&trial_start_time);
			break;

		case EXPERIMENT3_BUTTON:
			calibrate_b = 0; experiment2_b = 0; experiment3_b = 1;
			calibration_panel->disable(); experiment2_panel->disable(); experiment3_panel->enable();
			gaze_correction_x_spinner->enable(); gaze_correction_y_spinner->enable(); 
			experiment3_subject_panel->disable(); experiment3_image_panel->disable(); experiment3_eye_panel->disable();
			experiment3_sigma_spinner->disable(); experiment3_use_sigma_spinner->disable();
			experiment3_iris_spinner->enable();
			if ( experiment3_data_file.length() <= 1 ) experiment3_save_button->disable();
			break;

		case EXPERIMENT3_FILE_BROWSER:
			experiment3_show = 0;
			experiment3_file = "";
			experiment3_file = experiment3_file_browser->get_file();
			if ( readExperiment3Data(experiment3_file) ) {
				experiment3_save_button->enable();
				experiment3_trial_spinner->set_int_limits(0, experiment3_trials.size() - 1);
				experiment3_trial = 0;
				//experiment3_trial_spinner->set_int_val(experiment3_trial);
				setExperiment3State(experiment3_trial);
			}else {
				experiment3_save_button->disable();
			}
			break;

		case EXPERIMENT3_TRIAL_SPINNER:
			experiment3_show = 0;
			//if ( experiment3_trial-1 >= 0 )
			{
				saveExperiment3State(experiment3_trial_spinner->last_int_val);
				setExperiment3State(experiment3_trial);
			}
			break;

		case EXPERIMENT3_SAVE_BUTTON:
			saveExperiment3Data();
			break;

		case EXPERIMENT3_SHOW_CHECKBOX:
			if ( experiment3_show )
				time(&trial_start_time);
			break;

		default:
			break;
	}
	//glui->sync_live();
	GLUI_Master.sync_live_all();
}

/**********************************************************************
Function to process controller button presses for use selection
during Calibration. User actions are only processed while the
verification image is being shown.
**********************************************************************/
void ProcessCalibrateKeys( )
{
	if ( glfwJoystickPresent( GLFW_JOYSTICK_1 ) )
	{
		int button_count = 0;
		const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &button_count);

		if ( button_count == 14 )
		{
			//Select Button//
			if ( buttons[6] == GLFW_PRESS && !joybuttons[6] ) {
				joybuttons[6] = true;
				user_action = 1;
			}else if (buttons[6] == GLFW_RELEASE){
				joybuttons[6] = false;
			}
			
			//Up Button//
			if ( buttons[10] == GLFW_PRESS && !joybuttons[10] )	{
				joybuttons[10] = true;
				calibrate_y += .025f;
			}else if (buttons[10] == GLFW_RELEASE){
				joybuttons[10] = false;
			}

			//Down Button//
			if ( buttons[12] == GLFW_PRESS && !joybuttons[12] )	{
				joybuttons[12] = true;
				calibrate_y -= .025f;
			}else if (buttons[12] == GLFW_RELEASE){
				joybuttons[12] = false;
			}

			//Left Button//
			if ( buttons[13] == GLFW_PRESS && !joybuttons[13] )	{
				joybuttons[13] = true;
				calibrate_x -= .025f;
			}else if (buttons[13] == GLFW_RELEASE){
				joybuttons[13] = false;
			}

			//Right Button//
			if ( buttons[11] == GLFW_PRESS && !joybuttons[11] )	{
				joybuttons[11] = true;
				calibrate_x += .025f;
			}else if (buttons[11] == GLFW_RELEASE){
				joybuttons[11] = false;
			}
		}
	}
}

/**********************************************************************
Function to process controller button presses for use selection
during Experiment 2. User actions are only processed while the
image is being shown.
**********************************************************************/
void ProcessExperiment2Keys()
{
	if ( glfwJoystickPresent( GLFW_JOYSTICK_1 ) )
	{
		int button_count = 0;
		const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &button_count);

		if ( button_count == 14 )
		{
			//Select Button//
			if ( buttons[6] == GLFW_PRESS && !joybuttons[6] ) {
				joybuttons[6] = true;
				experiment2_show = 0;
				saveExperiment2State(experiment2_trial);
				setExperiment2State(++experiment2_trial);
			}else if (buttons[6] == GLFW_RELEASE){
				joybuttons[6] = false;
			}

			//Small-Up Button//
			if ( buttons[10] == GLFW_PRESS && !joybuttons[10] && buttons[4] == GLFW_PRESS )	{
				joybuttons[10] = true;
				experiment2_user_sigma = experiment2_user_sigma + 0.1f < 40.1f ? experiment2_user_sigma + 0.1f : 40.0f;
				//experiment2_sigma = images[experiment2_image].UpdateSigma(experiment2_pupil, experiment2_distance);
				images[experiment2_image].width_of_blur = experiment2_user_sigma;
				images[experiment2_image].DeconvolveImg();//experiment2_user_sigma);
			}else if (buttons[10] == GLFW_RELEASE){
				joybuttons[10] = false;
			}

			//Small-Down Button//
			if ( buttons[12] == GLFW_PRESS && !joybuttons[12] && buttons[4] == GLFW_PRESS )	{
				joybuttons[12] = true;
				experiment2_user_sigma = experiment2_user_sigma - 0.1f > 0.4f ? experiment2_user_sigma - 0.1f : 0.5f;
				//experiment2_sigma = images[experiment2_image].UpdateSigma(experiment2_pupil, experiment2_distance);
				images[experiment2_image].width_of_blur = experiment2_user_sigma;
				images[experiment2_image].DeconvolveImg();//experiment2_user_sigma);
			}else if (buttons[12] == GLFW_RELEASE){
				joybuttons[12] = false;
			}

			//Small-Right Button//
			if ( buttons[11] == GLFW_PRESS && !joybuttons[11] && buttons[4] == GLFW_PRESS )	{
				joybuttons[11] = true;
				experiment2_user_sigma = experiment2_user_sigma + 0.5f < 40.1f ? experiment2_user_sigma + 0.5f : 40.0f;
				//experiment2_sigma = images[experiment2_image].UpdateSigma(experiment2_pupil, experiment2_distance);
				images[experiment2_image].width_of_blur = experiment2_user_sigma;
				images[experiment2_image].DeconvolveImg();//experiment2_user_sigma);
			}else if (buttons[11] == GLFW_RELEASE){
				joybuttons[11] = false;
			}

			//Small-Right Button//
			if ( buttons[13] == GLFW_PRESS && !joybuttons[13] && buttons[4] == GLFW_PRESS)	{
				joybuttons[13] = true;
				experiment2_user_sigma = experiment2_user_sigma - 0.5f > 0.0f ? experiment2_user_sigma - 0.5f : 0.5f;
				//experiment2_sigma = images[experiment2_image].UpdateSigma(experiment2_pupil, experiment2_distance);
				images[experiment2_image].width_of_blur = experiment2_user_sigma;
				images[experiment2_image].DeconvolveImg();//experiment2_user_sigma);
			}else if (buttons[13] == GLFW_RELEASE){
				joybuttons[13] = false;
			}
			
			//Up Button//
			if ( buttons[10] == GLFW_PRESS && !joybuttons[10] )	{
				joybuttons[10] = true;
				experiment2_user_sigma = experiment2_user_sigma + 1.0f < 40.1f ? experiment2_user_sigma + 1.0f : 40.0f;
				//experiment2_sigma = images[experiment2_image].UpdateSigma(experiment2_pupil, experiment2_distance);
				images[experiment2_image].width_of_blur = experiment2_user_sigma;
				images[experiment2_image].DeconvolveImg();//experiment2_user_sigma);
			}else if (buttons[10] == GLFW_RELEASE){
				joybuttons[10] = false;
			}

			//Down Button//
			if ( buttons[12] == GLFW_PRESS && !joybuttons[12] )	{
				joybuttons[12] = true;
				experiment2_user_sigma = experiment2_user_sigma - 1.0f > 0.4f ? experiment2_user_sigma - 1.0f : 0.4f;
				//experiment2_sigma = images[experiment2_image].UpdateSigma(experiment2_pupil, experiment2_distance);
				images[experiment2_image].width_of_blur = experiment2_user_sigma;
				images[experiment2_image].DeconvolveImg();//experiment2_user_sigma);
			}else if (buttons[12] == GLFW_RELEASE){
				joybuttons[12] = false;
			}

			//Right Button//
			if ( buttons[11] == GLFW_PRESS && !joybuttons[11] )	{
				joybuttons[11] = true;
				experiment2_user_sigma = experiment2_user_sigma + 5.0f < 40.1f ? experiment2_user_sigma + 5.0f : 40.0f;
				//experiment2_sigma = images[experiment2_image].UpdateSigma(experiment2_pupil, experiment2_distance);
				images[experiment2_image].width_of_blur = experiment2_user_sigma;
				images[experiment2_image].DeconvolveImg();//experiment2_user_sigma);
			}else if (buttons[11] == GLFW_RELEASE){
				joybuttons[11] = false;
			}

			//Left Button//
			if ( buttons[13] == GLFW_PRESS && !joybuttons[13] )	{
				joybuttons[13] = true;
				experiment2_user_sigma = experiment2_user_sigma - 5.0f > 0.4f ? experiment2_user_sigma - 5.0f : 0.5f;
				//experiment2_sigma = images[experiment2_image].UpdateSigma(experiment2_pupil, experiment2_distance);
				images[experiment2_image].width_of_blur = experiment2_user_sigma;
				images[experiment2_image].DeconvolveImg();//experiment2_user_sigma);
			}else if (buttons[13] == GLFW_RELEASE){
				joybuttons[13] = false;
			}
		}
	}
}

/**********************************************************************
Function to process controller button presses for use selection
during Experiment 3. User actions are only processed while the
image is being shown.
**********************************************************************/
void ProcessExperiment3Keys()
{
	if ( glfwJoystickPresent( GLFW_JOYSTICK_1 ) )
	{
		int button_count = 0;
		const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &button_count);

		if ( button_count == 14 )
		{
			//Select Button//
			if ( buttons[6] == GLFW_PRESS && !joybuttons[6] ) {
				joybuttons[6] = true;
				experiment3_show = 0;
				experiment3_match = 1;
				saveExperiment3State(experiment3_trial);
				setExperiment3State(++experiment3_trial);
			}else if (buttons[6] == GLFW_RELEASE){
				joybuttons[6] = false;
			}
			
			//Start Button//
			if ( buttons[7] == GLFW_PRESS && !joybuttons[7] ) {
				joybuttons[7] = true;
				experiment3_show = 0;
				experiment3_match = 0;
				saveExperiment3State(experiment3_trial);
				setExperiment3State(++experiment3_trial);
			}else if (buttons[7] == GLFW_RELEASE){
				joybuttons[7] = false;
			}
		}
	}
}

/**********************************************************************
Function which delegates the processing of the controller buttons to
the proper function specific to the current experiment state.

If the current active state is calibration: ProcessCalibrateKeys
If the current active state is experiment 2: ProcessExperiment2Keys
If the current active state is experiment 3: ProcessExperiment3Keys
**********************************************************************/
void ProcessKeys( )
{
	if ( calibrate_b )
		ProcessCalibrateKeys();
	else if ( experiment2_b )
		ProcessExperiment2Keys();
	else if ( experiment3_b )
		ProcessExperiment3Keys();
}

/**********************************************************************
Function that is performed during frame redraw when no action has
been made. The default action is to call the Display function for
the active experiment mode (glutPostRedisplay).
**********************************************************************/
void Idle( void )
{
  if ( glutGetWindow() != main_window ) 
    glutSetWindow(main_window);  

  glutPostRedisplay();
   
  glui->sync_live();
}

/**********************************************************************
Function to adjust the orthographic view matrix and viewport to
accomodate the new window size. Also calculates the new aspect ratio 
used in displaying the image square for experiment 2/3. Also ensures
that the window is displayed at the proper location to appear fullscreen
within the Moverio screen.
**********************************************************************/
void Reshape( int width, int height )
{
	//Calculate the new aspect ratio//
	aspect = (double)width / (double)height;

	//adjust the projection matrix to match the new aspect ratio//
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	if ( width > height ){
		gluOrtho2D(0.0, 1.0*aspect, 0.0, 1.0);
		WIDTH = aspect; HEIGHT = 1.0f;
	}else{
		gluOrtho2D(0.0, 1.0, 0.0, 1.0/aspect);
		WIDTH = 1.0f; HEIGHT = 1.0f/aspect;
	}

	glViewport( 0, 0, width, height );
	glMatrixMode( GL_MODELVIEW );
	
	//Make sure the window is located in the proper position//
	glutPositionWindow(-1280, 0);
	glutPostRedisplay();
}

/**********************************************************************
Render the calibration crosshairs to the screen along with the
verification image. The user is able to make a final adjustment to
the location of the verification image to ensure that it appears
offset from the center of vision.
**********************************************************************/
void DisplayCalibrate()
{
	//Process Key Presses//
	ProcessCalibrateKeys();

	//Clear the draw buffer values//
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	//Ensure that all previous transformations are cleared//
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	
	//Render the currently selected calibration crosshair//
	glPushMatrix();
	glTranslatef(crosshairlocations[calibrate_number].first*WIDTH, crosshairlocations[calibrate_number].second*HEIGHT, 0.0f);
	glBegin(GL_LINES);
			if ( user_action )
				glColor3f(0.0f, 1.0f, 0.0f);
			else
				glColor3f(1.0f, 0.0f, 0.0f);
			glVertex2f(crosshair.vertices[0].X, crosshair.vertices[0].Y);
			glVertex2f(crosshair.vertices[1].X, crosshair.vertices[1].Y);
			glVertex2f(crosshair.vertices[2].X, crosshair.vertices[2].Y);
			glVertex2f(crosshair.vertices[3].X, crosshair.vertices[3].Y);
	glEnd();
	glPopMatrix();

	//render the verification image//
	if ( render_verify )
	{
		//render the calibration image//
		glPushMatrix();
		glTranslatef(calibrate_x*WIDTH, calibrate_y*HEIGHT, 0.0f);
		//glTranslatef(pupil_comm.normx*WIDTH, pupil_comm.normy*HEIGHT, 0.0f);
		glBegin(GL_QUADS);
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3f(-SQUARE_SIZE, -SQUARE_SIZE, 0.0f);
			glVertex3f( SQUARE_SIZE, -SQUARE_SIZE, 0.0f);
			glVertex3f( SQUARE_SIZE,  SQUARE_SIZE, 0.0f);
			glVertex3f(-SQUARE_SIZE,  SQUARE_SIZE, 0.0f);
		glEnd();
		glPopMatrix();
	}

	//Swap Rendering Buffers//
	glutSwapBuffers(); 
}

/**********************************************************************
Render the image for the current Experiment 2 trial. The image should
be rendered slightly offset from where the user is looking. User
actions are also processed, but only while the image is viewable.
**********************************************************************/
void DisplayExperiment2()
{
	//clear the draw buffer values//
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	//ensure that all previous transformations are cleared//
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	switch(use_gaze_check)
	{
		case 1:
			//render the image for the current trial//
			if ( !(experiment2_show && !checkGaze()) )
			{
				break;
			}
		case 0:
			if ( experiment2_show )
			{
				//Process Key Presses//
				ProcessExperiment2Keys();
				experiment2_sigma = CalcSigma(experiment2_pupil, experiment2_distance);

				///////////////////////////////////////////////////////////////////////////////
				//Set Active Texture//
				glActiveTexture( GL_TEXTURE0 );
				glEnable( GL_TEXTURE_2D );
				glBindTexture( GL_TEXTURE_2D, image_texture );    
				//Load image file//
				glTexImage2D ( GL_TEXTURE_2D, 0, GL_LUMINANCE, images[experiment2_image].width, images[experiment2_image].height,
					0, GL_LUMINANCE, GL_UNSIGNED_BYTE, images[experiment2_image].deconvolution_result.data );

				//render the trial image//
				glPushMatrix();
				glTranslatef(calibrate_x*WIDTH, calibrate_y*HEIGHT, 0.0f);

				shader_image.Activate();
				glBegin(GL_QUADS);
					glTexCoord2f(0.0f, 1.0f);
					glVertex3f(-SQUARE_SIZE, -SQUARE_SIZE, 0.0f);
					glTexCoord2f(1.0f, 1.0f);
					glVertex3f( SQUARE_SIZE, -SQUARE_SIZE, 0.0f);
					glTexCoord2f(1.0f, 0.0f);
					glVertex3f( SQUARE_SIZE,  SQUARE_SIZE, 0.0f);
					glTexCoord2f(0.0f, 0.0f);
					glVertex3f(-SQUARE_SIZE,  SQUARE_SIZE, 0.0f);
				glEnd();
				shader_image.DeActivate();

				glPopMatrix();

				glDisable( GL_TEXTURE_2D );
				glBindTexture( GL_TEXTURE_2D, 0 );

				////////////////////////////////////////////////////////////////////////////////////
				//Set Active Texture//
				glActiveTexture( GL_TEXTURE0 );
				glEnable( GL_TEXTURE_2D );
				glBindTexture( GL_TEXTURE_2D, image_texture );    
				
				//Load image file//
				if ( experiment2_user_sigma <= 13.333 )
				{
					glTexImage2D ( GL_TEXTURE_2D, 0, GL_LUMINANCE, gauge_images[0].width, gauge_images[0].height,
						0, GL_LUMINANCE, GL_UNSIGNED_BYTE, gauge_images[1].src.data );
				}else if ( experiment2_user_sigma <= 26.666 )
				{
					glTexImage2D ( GL_TEXTURE_2D, 0, GL_LUMINANCE, gauge_images[0].width, gauge_images[0].height,
						0, GL_LUMINANCE, GL_UNSIGNED_BYTE, gauge_images[2].src.data );
				}else if ( experiment2_user_sigma <= 40.0 )
				{
					glTexImage2D ( GL_TEXTURE_2D, 0, GL_LUMINANCE, gauge_images[0].width, gauge_images[0].height,
						0, GL_LUMINANCE, GL_UNSIGNED_BYTE, gauge_images[3].src.data );
				}else
				{
					glTexImage2D ( GL_TEXTURE_2D, 0, GL_LUMINANCE, gauge_images[0].width, gauge_images[0].height,
						0, GL_LUMINANCE, GL_UNSIGNED_BYTE, gauge_images[0].src.data );
				}


				//render the gauge image//
				glPushMatrix();
				glTranslatef(calibrate_x*WIDTH + SQUARE_SIZE*2 + 0.05, calibrate_y*HEIGHT - SQUARE_SIZE - SQUARE_SIZE/8, 0.0f);

				shader_image.Activate();
				/*glBegin(GL_QUADS);
					glTexCoord2f(0.0f, 1.0f);
					glVertex3f(-SQUARE_SIZE, -SQUARE_SIZE/16, 0.0f);
					glTexCoord2f(1.0f, 1.0f);
					glVertex3f( SQUARE_SIZE, -SQUARE_SIZE/16, 0.0f);
					glTexCoord2f(1.0f, 0.0f);
					glVertex3f( SQUARE_SIZE,  SQUARE_SIZE/16, 0.0f);
					glTexCoord2f(0.0f, 0.0f);
					glVertex3f(-SQUARE_SIZE,  SQUARE_SIZE/16, 0.0f);
				glEnd();*/
				shader_image.DeActivate();

				glPopMatrix();

				glDisable( GL_TEXTURE_2D );
				glBindTexture( GL_TEXTURE_2D, 0 );
			}
			break;
	}

	//swap the render buffers to display the scene//
	glutSwapBuffers();
}

/**********************************************************************
Render the image for the current Experiment 3 trial. The image should
be rendered slightly offset from where the user is looking. User
actions are also processed, but only while the image is viewable.
**********************************************************************/
void DisplayExperiment3()
{
	//clear the draw buffer values//
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	//ensure that all previous transformations are cleared//
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	switch(use_gaze_check)
	{
		case 1:
			//render the image for the current trial//
			if ( !(experiment3_show && !checkGaze()) )
			{
				break;
			}
		case 0:
			if ( experiment3_show )
			{
				float tsigma = CalcSigma(experiment3_pupil, experiment3_distance);
				if ( abs(tsigma - experiment3_sigma) >= 2 )
				{
					experiment3_sigma = images[experiment3_image].width_of_blur = tsigma;
					//images[experiment3_image].DeconvolveImg();
				}

				//Process Key Presses//
				ProcessExperiment3Keys();

				//Set Active Texture//
				glActiveTexture( GL_TEXTURE0 );
				glEnable( GL_TEXTURE_2D );
				glBindTexture( GL_TEXTURE_2D, image_texture );    
				//Load image file//
				if ( experiment3_use_sigma )
					glTexImage2D ( GL_TEXTURE_2D, 0, GL_LUMINANCE, images[experiment3_image].width, images[experiment3_image].height,
						0, GL_LUMINANCE, GL_UNSIGNED_BYTE, images[experiment3_image].deconvolution_result.data );
				else
					glTexImage2D ( GL_TEXTURE_2D, 0, GL_LUMINANCE, images[experiment3_image].width, images[experiment3_image].height,
						0, GL_LUMINANCE, GL_UNSIGNED_BYTE, images[experiment3_image].src.data );

				//render the image//
				glPushMatrix();
				glTranslatef(calibrate_x*WIDTH, calibrate_y*HEIGHT, 0.0f);
				//glTranslatef(pupil_comm.normx*WIDTH, pupil_comm.normy*HEIGHT, 0.0f);

				shader_image.Activate();
				glBegin(GL_QUADS);
					glTexCoord2f(0.0f, 1.0f);
					glVertex3f(-SQUARE_SIZE, -SQUARE_SIZE, 0.0f);
					glTexCoord2f(1.0f, 1.0f);
					glVertex3f( SQUARE_SIZE, -SQUARE_SIZE, 0.0f);
					glTexCoord2f(1.0f, 0.0f);
					glVertex3f( SQUARE_SIZE,  SQUARE_SIZE, 0.0f);
					glTexCoord2f(0.0f, 0.0f);
					glVertex3f(-SQUARE_SIZE,  SQUARE_SIZE, 0.0f);
				glEnd();
				shader_image.DeActivate();

				glPopMatrix();

				glDisable( GL_TEXTURE_2D );
				glBindTexture( GL_TEXTURE_2D, 0 );
			}
			break;
	}

	//swap the render buffers to display the scene//
	glutSwapBuffers();
}

/**********************************************************************
Display function which delegates rendering to the appropriate function
depending on the active experiment state.

If Calibration is active: DisplayCalibrate
If Experiment 2 is active: DisplayExperiment2
If Experiment 3 is active: DisplayExperiment3
**********************************************************************/
void Display( void )
{
	//Process Pupil Software Messages//
	processEyeMessages();

	//Decide Which State to be Rendered//
	if ( calibrate_b )
		DisplayCalibrate();
	else if ( experiment2_b )
		DisplayExperiment2();
	else if ( experiment3_b )
		DisplayExperiment3();
}

/**********************************************************************
Main function and starting point of the program.

The OpenGL rendering window is setup and the GLUI instance created.
The GLUI controls are also created and the experiment objects created.

The crosshair location specified, images loaded, ZMQ interface activated,
and the main rendering loop started.
**********************************************************************/
int main(int argc, char* argv[])
{
	/****************************************/
	/*   Initialize GLUT and create window  */
	/****************************************/
	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowPosition( 0, 0 );
	glutInitWindowSize( 1280, 720 );
 
	main_window = glutCreateWindow( "SharpView Display Window" );
	glutDisplayFunc( Display );
	glutReshapeFunc( Reshape );  

	//Initialize GLEW OEPNGL extensions//
	glewInit( );

	//Openg GL Enables and Features//
	glEnable(GL_DEPTH_TEST);
	glLineWidth(3.0f);

	//Load shader for rendering image as texture on the square//
	shader_image.create_shaders("square.vert", "square.frag");
	shader_image.Activate();
	shader_image.set_uniform_1i("Texture", 0);
	shader_image.DeActivate();

	//Load image texture//
    glGenTextures( 1, &image_texture );
    glBindTexture( GL_TEXTURE_2D, image_texture );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    //Load image file
	// glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGB, image_texture.width, image_texture.height,
	//     0, GL_RGB, GL_UNSIGNED_BYTE, &image_texture.rgb_array[0] );
    glBindTexture( GL_TEXTURE_2D, 0 );

	/****************************************/
	/*         Here's the GLUI code         */
	/****************************************/
	printf( "GLUI version: %3.2f\n", GLUI_Master.get_version() );
	//Create Glui Window//
	glui = GLUI_Master.create_glui( "SharpView Experiment Controls", 0, 400, 50 );
	new GLUI_StaticText( glui, "SharpView" ); 
	
	//////////////////////////////////////////////////////////////////////////
	//Add Global Controls//
	use_gaze_check_checkbox = new GLUI_Checkbox(glui, "Use Gaze Check", &use_gaze_check);

	//////////////////////////////////////////////////////////////////////////
	//Add Experiment Button Panel//
	experiment_panel = new GLUI_Panel(glui, "Experiments" );

	//Buttons to Select Experiment State//
	calibrate_button = new GLUI_Button(experiment_panel, "Calibrate", CALIBRATE_BUTTON, control_cb);
	calibrate_checkbox = new GLUI_Checkbox(experiment_panel, "", &calibrate_b, CALIBRATE_CHECKBOX, control_cb );
	calibrate_checkbox->disable();
	new GLUI_Column( experiment_panel, false );
	experiment2_button = new GLUI_Button(experiment_panel, "Experiment2", EXPERIMENT2_BUTTON, control_cb);
	experiment2_checkbox = new GLUI_Checkbox(experiment_panel, "", &experiment2_b, EXPERIMENT2_CHECKBOX, control_cb );
	experiment2_checkbox->disable();
	new GLUI_Column( experiment_panel, false );
	experiment3_button = new GLUI_Button(experiment_panel, "Experiment3", EXPERIMENT3_BUTTON, control_cb);
	experiment3_checkbox = new GLUI_Checkbox(experiment_panel, "", &experiment3_b, EXPERIMENT3_CHECKBOX, control_cb );
	experiment3_checkbox->disable();
	
	///////////////////////////////////////////////////////////////////////////
	//Add Calibrate Panel//
	calibration_panel = new GLUI_Panel(glui, "Calibrate");
	calibration_panel->disable();

	//Glui controls to facilitate calibration and onscreen item correction//
	render_verification_checkbox = new GLUI_Checkbox(calibration_panel, "Render Verify", &render_verify, RENDER_VERIFICATION_CHECKBOX, control_cb );
	user_action_checkbox = new GLUI_Checkbox(calibration_panel, "User Response", &user_action, USER_ACTION_CHECKBOX, control_cb );
	user_action_checkbox->disable();
	calibrate_number_spinner = new GLUI_Spinner( calibration_panel, "Calib Num", &calibrate_number, CALIBRATE_NUMBER_SPINNER, control_cb);
	calibrate_number_spinner->set_int_limits( 0, 9 );
	new GLUI_Column( calibration_panel, false );
	calibrate_x_spinner = new GLUI_Spinner( calibration_panel, "CalibX", &calibrate_x);
	calibrate_x_spinner->set_float_limits( -1.0f, 1.0 );
	calibrate_x_spinner->disable();
	calibrate_y_spinner = new GLUI_Spinner( calibration_panel, "CalibY", &calibrate_y);
	calibrate_y_spinner->set_float_limits( -1.0f, 1.0 );
	calibrate_y_spinner->disable();
	new GLUI_Column( calibration_panel, false );
	gaze_correction_x_spinner = new GLUI_Spinner( calibration_panel, "Gaze X", &gaze_correction_x);
	gaze_correction_x_spinner->set_float_limits( -10.0f, 20.0 );
	gaze_correction_x_spinner->set_float_val(0.0f);
	//gaze_correction_x_spinner->disable();
	gaze_correction_y_spinner = new GLUI_Spinner( calibration_panel, "Gaze Y", &gaze_correction_y);
	gaze_correction_y_spinner->set_float_limits( -10.0f, 10.0 );
	gaze_correction_y_spinner->set_float_val(0.0f);
	//gaze_correction_y_spinner->disable();
	
	//////////////////////////////////////////////////////////////////////////
	//Add Experiment2 Panel//
	experiment2_panel = new GLUI_Panel(glui, "Experiment2");
	experiment2_panel->disable();

	experiment2_file_browser = new GLUI_FileBrowser(experiment2_panel, "Experiment 2 Subject File", false, EXPERIMENT2_FILE_BROWSER, control_cb);
	experiment2_file_browser->set_h(180);
	new GLUI_Column( experiment2_panel, false );
	experiment2_subject_panel = new GLUI_Panel(experiment2_panel, "");
	experiment2_subject_spinner = new GLUI_Spinner( experiment2_subject_panel, "Subject", &experiment2_subject );
	experiment2_subject_spinner->set_int_val(-1);
	new GLUI_Column( experiment2_subject_panel, false );
	experiment2_distance_spinner = new GLUI_Spinner( experiment2_subject_panel, "Distance", &experiment2_distance );
	experiment2_distance_spinner->set_int_val(-1);
	experiment2_trial_panel = new GLUI_Panel(experiment2_panel, "");
	experiment2_trial_spinner = new GLUI_Spinner(experiment2_trial_panel, "Trial", &experiment2_trial,EXPERIMENT2_TRIAL_SPINNER, control_cb);
	experiment2_trial_spinner->set_int_limits(0, 0);
	new GLUI_Column( experiment2_trial_panel, false );
	experiment2_image_spinner = new GLUI_Spinner(experiment2_trial_panel, "Image", &experiment2_image);
	experiment2_image_spinner->set_int_val(0);
	experiment2_eye_panel = new GLUI_Panel(experiment2_panel, "");
	experiment2_iris_spinner = new GLUI_Spinner(experiment2_eye_panel, "Iris", &experiment2_iris);
	experiment2_iris_spinner->set_int_limits(0, 500);
	experiment2_iris_spinner->set_int_val(210);
	new GLUI_Column( experiment2_eye_panel, false );
	experiment2_pupil_spinner = new GLUI_Spinner(experiment2_eye_panel, "Pupil", &experiment2_pupil);
	experiment2_pupil_spinner->set_float_limits(0.0f, 15.0f);
	experiment2_pupil_spinner->set_float_val(0.0);
	experiment2_sigma_panel = new GLUI_Panel(experiment2_panel, "");
	experiment2_user_sigma_spinner = new GLUI_Spinner(experiment2_sigma_panel, "User Sigma", &experiment2_user_sigma, EXPERIMENT2_USER_SIGMA_SPINNER, control_cb);
	experiment2_user_sigma_spinner->set_float_limits(0.5f, 40.0);
	experiment2_user_sigma_spinner->set_float_val(0.5);
	new GLUI_Column( experiment2_sigma_panel, false );
	experiment2_sigma_spinner = new GLUI_Spinner(experiment2_sigma_panel, "Sigma", &experiment2_sigma);
	experiment2_sigma_spinner->set_float_limits(0.0f, 40.0);
	experiment2_sigma_spinner->set_float_val(0.0f);
	experiment2_show_save_panel = new GLUI_Panel(experiment2_panel, "");
	experiment2_show_checkbox = new GLUI_Checkbox(experiment2_show_save_panel, "Show Image", &experiment2_show, EXPERIMENT2_SHOW_CHECKBOX, control_cb );
	new GLUI_Column( experiment2_show_save_panel, false );
	experiment2_save_button = new GLUI_Button(experiment2_show_save_panel, "Save Data", EXPERIMENT2_SAVE_BUTTON, control_cb);

	//////////////////////////////////////////////////////////////////////////
	//Add Experiment3 Panel//
	experiment3_panel = new GLUI_Panel(glui, "Experiment3");
	experiment3_panel->disable();

	experiment3_file_browser = new GLUI_FileBrowser(experiment3_panel, "Experiment 3 Subject File", false, EXPERIMENT3_FILE_BROWSER, control_cb);
	experiment3_file_browser->set_h(180);
	new GLUI_Column( experiment3_panel, false );
	experiment3_subject_panel = new GLUI_Panel(experiment3_panel, "");
	experiment3_subject_spinner = new GLUI_Spinner( experiment3_subject_panel, "Subject", &experiment3_subject );
	experiment3_subject_spinner->set_int_val(-1);
	new GLUI_Column( experiment3_subject_panel, false );
	experiment3_distance_spinner = new GLUI_Spinner( experiment3_subject_panel, "Distance", &experiment3_distance );
	experiment3_distance_spinner->set_int_val(-1);
	experiment3_trial_panel = new GLUI_Panel(experiment3_panel, "");
	experiment3_trial_spinner = new GLUI_Spinner(experiment3_trial_panel, "Trial", &experiment3_trial,EXPERIMENT3_TRIAL_SPINNER, control_cb);
	experiment3_trial_spinner->set_int_limits(0, 0);
	new GLUI_Column( experiment3_trial_panel, false );
	experiment3_sigma_spinner = new GLUI_Spinner(experiment3_trial_panel, "Sigma", &experiment3_sigma);
	experiment3_sigma_spinner->set_float_limits(0.0f, 40.0f);
	experiment3_sigma_spinner->set_float_val(0.0f);
	new GLUI_Column( experiment3_trial_panel, false );
	experiment3_use_sigma_spinner = new GLUI_Spinner(experiment3_trial_panel, "Use Sigma", &experiment3_use_sigma);
	experiment3_use_sigma_spinner->set_int_limits(0, 1);
	experiment3_eye_panel = new GLUI_Panel(experiment3_panel, "");
	experiment3_iris_spinner = new GLUI_Spinner(experiment3_eye_panel, "Iris", &experiment3_iris);
	experiment3_iris_spinner->set_int_limits(0, 500);
	experiment3_iris_spinner->set_int_val(210);
	new GLUI_Column( experiment3_eye_panel, false );
	experiment3_pupil_spinner = new GLUI_Spinner(experiment3_eye_panel, "Pupil", &experiment3_pupil);
	experiment3_pupil_spinner->set_float_limits(0.0f, 15.0f);
	experiment3_pupil_spinner->set_float_val(0.0);
	experiment3_image_panel = new GLUI_Panel(experiment3_panel, "");
	experiment3_image_spinner = new GLUI_Spinner(experiment3_image_panel, "Image", &experiment3_image);
	experiment3_image_spinner->set_int_val(0);
	new GLUI_Column( experiment3_image_panel, false );
	experiment3_image_spinner = new GLUI_Spinner(experiment3_image_panel, "Real Image", &experiment3_real_image);
	experiment3_image_spinner->set_float_val(0);
	experiment3_show_save_panel = new GLUI_Panel(experiment3_panel, "");
	experiment3_show_checkbox = new GLUI_Checkbox(experiment3_show_save_panel, "Show Image", &experiment3_show, EXPERIMENT3_SHOW_CHECKBOX, control_cb );
	new GLUI_Column( experiment3_show_save_panel, false );
	experiment3_save_button = new GLUI_Button(experiment3_show_save_panel, "Save Data", EXPERIMENT3_SAVE_BUTTON, control_cb);

	///////////////////////////////////////////////////////////////////////
	/**** Link windows to GLUI, and register idle callback ******/  
	glui->set_main_gfx_window( main_window );

	/* We register the idle callback with GLUI, not with GLUT */
	GLUI_Master.set_glutIdleFunc( Idle );

	////////////////////////////////////////////////////////////////////////////////
	if ( !glfwInit() )
		cout << "GLFW Not Initialized" << endl;
  
	for ( int i = 0 ; i < 14; i++ )
		joybuttons[i] = false;

	//Conect to ZMQ Network//
	pupil_comm.SetupSocketConnection();

	BuildCrossHairLocations();

	LoadImages();

	////////////////////////////////////////////////////////////////////////////////
	/**** Regular GLUT main loop ****/  
	glutMainLoop();

	////////////////////////////////////////////////////////////////////////////////
	glfwTerminate();
	return EXIT_SUCCESS;
}





