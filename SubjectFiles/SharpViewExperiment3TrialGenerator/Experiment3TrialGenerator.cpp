
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <ctime>

using namespace std;

vector<int> screen_image;
vector<int> real_image;

vector<vector<int>> vector_91011;
vector<vector<int>> vector_91012;
vector<vector<int>> vector_91112;
vector<vector<int>> vector_101112;

void buildVectors()
{
	vector<int> tvec(3);
	tvec[0] = 11; tvec[1] = 10; tvec[2] = 9;
	vector_91011.push_back(tvec);
	tvec[0] = 10; tvec[1] = 11; tvec[2] = 9;
	vector_91011.push_back(tvec);
	tvec[0] = 11; tvec[1] = 9; tvec[2] = 10;
	vector_91011.push_back(tvec);
	tvec[0] = 9; tvec[1] = 10; tvec[2] = 11;
	vector_91011.push_back(tvec);
	tvec[0] = 9; tvec[1] = 11; tvec[2] = 10;
	vector_91011.push_back(tvec);
	tvec[0] = 10; tvec[1] = 9; tvec[2] = 11;
	vector_91011.push_back(tvec);
	

	tvec[0] = 10; tvec[1] = 12; tvec[2] = 9;
	vector_91012.push_back(tvec);
	tvec[0] = 12; tvec[1] = 9; tvec[2] = 10;
	vector_91012.push_back(tvec);
	tvec[0] = 9; tvec[1] = 12; tvec[2] = 10;
	vector_91012.push_back(tvec);
	tvec[0] = 12; tvec[1] = 10; tvec[2] = 9;
	vector_91012.push_back(tvec);
	tvec[0] = 10; tvec[1] = 9; tvec[2] = 12;
	vector_91012.push_back(tvec);
	tvec[0] = 9; tvec[1] = 10; tvec[2] = 12;
	vector_91012.push_back(tvec);
	

	tvec[0] = 12; tvec[1] = 11; tvec[2] = 9;
	vector_91112.push_back(tvec);
	tvec[0] = 11; tvec[1] = 12; tvec[2] = 9;
	vector_91112.push_back(tvec);
	tvec[0] = 9; tvec[1] = 12; tvec[2] = 11;
	vector_91112.push_back(tvec);
	tvec[0] = 12; tvec[1] = 9; tvec[2] = 11;
	vector_91112.push_back(tvec);
	tvec[0] = 9; tvec[1] = 11; tvec[2] = 12;
	vector_91112.push_back(tvec);
	tvec[0] = 11; tvec[1] = 9; tvec[2] = 12;
	vector_91112.push_back(tvec);
	

	tvec[0] = 12; tvec[1] = 11; tvec[2] = 10;
	vector_101112.push_back(tvec);
	tvec[0] = 11; tvec[1] = 10; tvec[2] = 12;
	vector_101112.push_back(tvec);
	tvec[0] = 12; tvec[1] = 10; tvec[2] = 11;
	vector_101112.push_back(tvec);
	tvec[0] = 10; tvec[1] = 11; tvec[2] = 12;
	vector_101112.push_back(tvec);
	tvec[0] = 11; tvec[1] = 12; tvec[2] = 10;
	vector_101112.push_back(tvec);
	tvec[0] = 10; tvec[1] = 12; tvec[2] = 11;
	vector_101112.push_back(tvec);
}

int main(int argv, char** argc)
{
	vector<int> pair_loc;
	pair_loc.push_back(0);
	pair_loc.push_back(1);
	pair_loc.push_back(2);
	pair_loc.push_back(3);
	pair_loc.push_back(4);
	pair_loc.push_back(5);

	srand(time(0));
	buildVectors();

	int distance_level = 500;
	string sdistance_level = "500";
	int subject = 16;
	string ssubject = "16";
	ifstream rimgorder("S"+ssubject+"-"+sdistance_level+".txt");
	while (!rimgorder.eof())
	{
		int token;
		rimgorder >> token;
		screen_image.push_back(token);
	}

	vector<int> nine;
	vector<int> ten;
	vector<int> eleven;
	vector<int> twelve;

	int rloc = 0;
	switch (distance_level)
	{
		case 25:
			rloc = rand()%pair_loc.size();
			nine.push_back(pair_loc[rloc]);
			pair_loc.erase(pair_loc.begin()+rloc);
			rloc = rand()%pair_loc.size();
			nine.push_back(pair_loc[rloc]);
			pair_loc.erase(pair_loc.begin()+rloc);

			rloc = rand()%pair_loc.size();
			ten.push_back(pair_loc[rloc]);
			pair_loc.erase(pair_loc.begin()+rloc);
			rloc = rand()%pair_loc.size();
			ten.push_back(pair_loc[rloc]);
			pair_loc.erase(pair_loc.begin()+rloc);

			rloc = rand()%pair_loc.size();
			eleven.push_back(pair_loc[rloc]);
			pair_loc.erase(pair_loc.begin()+rloc);
			rloc = rand()%pair_loc.size();
			eleven.push_back(pair_loc[rloc]);
			pair_loc.erase(pair_loc.begin()+rloc);

			for (int i = 0; i < screen_image.size(); i++)
			{
				switch(screen_image[i])
				{
					case 9:
						if ( vector_91011[nine.back()].size() > 0 )
						{
							real_image.push_back(vector_91011[nine.back()].back());
							vector_91011[nine.back()].pop_back();
						}
						else
						{
							nine.pop_back();
							real_image.push_back(vector_91011[nine.back()].back());
							vector_91011[nine.back()].pop_back();
						}
						break;

					case 10:
						if ( vector_91011[ten.back()].size() > 0 )
						{
							real_image.push_back(vector_91011[ten.back()].back());
							vector_91011[ten.back()].pop_back();
						}
						else
						{
							ten.pop_back();
							real_image.push_back(vector_91011[ten.back()].back());
							vector_91011[ten.back()].pop_back();
						}
						break;

					case 11:
						if ( vector_91011[eleven.back()].size() > 0 )
						{
							real_image.push_back(vector_91011[eleven.back()].back());
							vector_91011[eleven.back()].pop_back();
						}
						else
						{
							eleven.pop_back();
							real_image.push_back(vector_91011[eleven.back()].back());
							vector_91011[eleven.back()].pop_back();
						}
						break;
				}
			}
			break;

		case 50:
			rloc = rand()%pair_loc.size();
			nine.push_back(pair_loc[rloc]);
			pair_loc.erase(pair_loc.begin()+rloc);
			rloc = rand()%pair_loc.size();
			nine.push_back(pair_loc[rloc]);
			pair_loc.erase(pair_loc.begin()+rloc);

			rloc = rand()%pair_loc.size();
			ten.push_back(pair_loc[rloc]);
			pair_loc.erase(pair_loc.begin()+rloc);
			rloc = rand()%pair_loc.size();
			ten.push_back(pair_loc[rloc]);
			pair_loc.erase(pair_loc.begin()+rloc);

			rloc = rand()%pair_loc.size();
			twelve.push_back(pair_loc[rloc]);
			pair_loc.erase(pair_loc.begin()+rloc);
			rloc = rand()%pair_loc.size();
			twelve.push_back(pair_loc[rloc]);
			pair_loc.erase(pair_loc.begin()+rloc);

			for (int i = 0; i < screen_image.size(); i++)
			{
				switch(screen_image[i])
				{
					case 9:
						if ( vector_91012[nine.back()].size() > 0 )
						{
							real_image.push_back(vector_91012[nine.back()].back());
							vector_91012[nine.back()].pop_back();
						}
						else
						{
							nine.pop_back();
							real_image.push_back(vector_91012[nine.back()].back());
							vector_91012[nine.back()].pop_back();
						}
						break;

					case 10:
						if ( vector_91012[ten.back()].size() > 0 )
						{
							real_image.push_back(vector_91012[ten.back()].back());
							vector_91012[ten.back()].pop_back();
						}
						else
						{
							ten.pop_back();
							real_image.push_back(vector_91012[ten.back()].back());
							vector_91012[ten.back()].pop_back();
						}
						break;

					case 12:
						if ( vector_91012[twelve.back()].size() > 0 )
						{
							real_image.push_back(vector_91012[twelve.back()].back());
							vector_91012[twelve.back()].pop_back();
						}
						else
						{
							twelve.pop_back();
							real_image.push_back(vector_91012[twelve.back()].back());
							vector_91012[twelve.back()].pop_back();
						}
						break;
				}
			}
			break;

		case 100:
			rloc = rand()%pair_loc.size();
			nine.push_back(pair_loc[rloc]);
			pair_loc.erase(pair_loc.begin()+rloc);
			rloc = rand()%pair_loc.size();
			nine.push_back(pair_loc[rloc]);
			pair_loc.erase(pair_loc.begin()+rloc);

			rloc = rand()%pair_loc.size();
			eleven.push_back(pair_loc[rloc]);
			pair_loc.erase(pair_loc.begin()+rloc);
			rloc = rand()%pair_loc.size();
			eleven.push_back(pair_loc[rloc]);
			pair_loc.erase(pair_loc.begin()+rloc);

			rloc = rand()%pair_loc.size();
			twelve.push_back(pair_loc[rloc]);
			pair_loc.erase(pair_loc.begin()+rloc);
			rloc = rand()%pair_loc.size();
			twelve.push_back(pair_loc[rloc]);
			pair_loc.erase(pair_loc.begin()+rloc);

			for (int i = 0; i < screen_image.size(); i++)
			{
				switch(screen_image[i])
				{
					case 9:
						if ( vector_91112[nine.back()].size() > 0 )
						{
							real_image.push_back(vector_91112[nine.back()].back());
							vector_91112[nine.back()].pop_back();
						}
						else
						{
							nine.pop_back();
							real_image.push_back(vector_91112[nine.back()].back());
							vector_91112[nine.back()].pop_back();
						}
						break;

					case 11:
						if ( vector_91112[eleven.back()].size() > 0 )
						{
							real_image.push_back(vector_91112[eleven.back()].back());
							vector_91112[eleven.back()].pop_back();
						}
						else
						{
							eleven.pop_back();
							real_image.push_back(vector_91112[eleven.back()].back());
							vector_91112[eleven.back()].pop_back();
						}
						break;

					case 12:
						if ( vector_91112[twelve.back()].size() > 0 )
						{
							real_image.push_back(vector_91112[twelve.back()].back());
							vector_91112[twelve.back()].pop_back();
						}
						else
						{
							twelve.pop_back();
							real_image.push_back(vector_91112[twelve.back()].back());
							vector_91112[twelve.back()].pop_back();
						}
						break;
				}
			}
			break;

		case 500:
			rloc = rand()%pair_loc.size();
			ten.push_back(pair_loc[rloc]);
			pair_loc.erase(pair_loc.begin()+rloc);
			rloc = rand()%pair_loc.size();
			ten.push_back(pair_loc[rloc]);
			pair_loc.erase(pair_loc.begin()+rloc);

			rloc = rand()%pair_loc.size();
			eleven.push_back(pair_loc[rloc]);
			pair_loc.erase(pair_loc.begin()+rloc);
			rloc = rand()%pair_loc.size();
			eleven.push_back(pair_loc[rloc]);
			pair_loc.erase(pair_loc.begin()+rloc);

			rloc = rand()%pair_loc.size();
			twelve.push_back(pair_loc[rloc]);
			pair_loc.erase(pair_loc.begin()+rloc);
			rloc = rand()%pair_loc.size();
			twelve.push_back(pair_loc[rloc]);
			pair_loc.erase(pair_loc.begin()+rloc);

			for (int i = 0; i < screen_image.size(); i++)
			{
				switch(screen_image[i])
				{
					case 10:
						if ( vector_101112[ten.back()].size() > 0 )
						{
							real_image.push_back(vector_101112[ten.back()].back());
							vector_101112[ten.back()].pop_back();
						}
						else
						{
							ten.pop_back();
							real_image.push_back(vector_101112[ten.back()].back());
							vector_101112[ten.back()].pop_back();
						}
						break;

					case 11:
						if ( vector_101112[eleven.back()].size() > 0 )
						{
							real_image.push_back(vector_101112[eleven.back()].back());
							vector_101112[eleven.back()].pop_back();
						}
						else
						{
							eleven.pop_back();
							real_image.push_back(vector_101112[eleven.back()].back());
							vector_101112[eleven.back()].pop_back();
						}
						break;

					case 12:
						if ( vector_101112[twelve.back()].size() > 0 )
						{
							real_image.push_back(vector_101112[twelve.back()].back());
							vector_101112[twelve.back()].pop_back();
						}
						else
						{
							twelve.pop_back();
							real_image.push_back(vector_101112[twelve.back()].back());
							vector_101112[twelve.back()].pop_back();
						}
						break;
				}
			}
			break;
	}

	ofstream tFile("S"+ssubject + "-"+sdistance_level+".ex3");
	tFile << "sub:\t" << ssubject << "dis:\t" << sdistance_level << endl;
	tFile << "img:\t0\t0\t0" << endl;
	for (int i = 0; i < screen_image.size(); i++)
	{
		tFile << "img:\t" << screen_image[i] << "\t" << real_image[i] << endl;
	}
	tFile << "img:\t0\t0\t0" << endl;
	return 0;
}