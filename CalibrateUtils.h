
#ifndef __CALIB_UTILS__
#define __CALIB_UTILS__

class VERTEX{
	public:
		VERTEX() : X(0.0f), Y(0.0f), Z(0.0f)
		{	}

		~VERTEX()
		{	}

		float X;
		float Y;
		float Z;

	private:

};

class CROSSHAIR{
	public:
		CROSSHAIR()
		{
			vertices[0].X = -0.025f;
			vertices[0].Y = 0.0f;
			vertices[0].Z = 0.0f;

			vertices[1].X = 0.025f;
			vertices[1].Y = 0.0f;
			vertices[1].Z = 0.0f;

			vertices[2].X = 0.0f;
			vertices[2].Y = 0.025f;
			vertices[2].Z = 0.0f;

			vertices[3].X = 0.0f;
			vertices[3].Y = -0.025f;
			vertices[3].Z = 0.0f;
		}

		~CROSSHAIR()
		{	}

		VERTEX vertices[4];

	private:

};


#endif __CALIB_UTILS
