//  MIT License
// 	Copyright(c) 2020 ChenKB
//
// 	Permission is hereby granted,
// 	free of charge, to any person obtaining a copy of this software and associated documentation files(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions :
//
// 	The above copyright notice and this permission notice shall be included in all copies
// 	or
// 	substantial portions of the Software.
//
// 	THE SOFTWARE IS PROVIDED "AS IS",
// 	WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// 	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// 	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// 	DAMAGES OR OTHER
// 	LIABILITY,
// 	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// 	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// 	SOFTWARE.

#include <iostream> // cout
#include <stdio.h>  // sprintf 
#include <stdlib.h> // system

#include <Eigen/Dense>
#include <opencv2/opencv.hpp>

// Required: CMake, opencv
// To generate executable:
// cmake . -DOpenCV_DIR="/path/to/opencv/build/folder"
// Afterwards:
// make (Do this to recompile)

using namespace Eigen;
using namespace std;

const int plateSize = 25; // How many balls in each side of the square?

// A ball.
struct grid_unit
{
	Vector3d pos;
	Vector3d v;
	Vector3d f;
	float m;
};

// Set up all the balls' position, velocity and force to 0.
void initialize_layer(int gridSize, grid_unit (*layer)[plateSize][plateSize], float y)
{
	for (int i = 0; i < plateSize; i++)
	{
		for (int j = 0; j < plateSize; j++)
		{
			(*layer)[i][j].pos << i * gridSize, y, j * gridSize;
			(*layer)[i][j].v << 0, 0, 0;
			(*layer)[i][j].f << 0, 0, 0;
			(*layer)[i][j].m = 1;
		}
	}
}

// Calculates layer0[i][j]'s force given one other ball's position
Vector3d spring_f(int i, int j, int i0, int j0,
				  grid_unit (*layer1)[plateSize][plateSize],
				  grid_unit (*layer0)[plateSize][plateSize],
				  bool is_same_lyr, float gridSize)
{
	double length0;
	if (is_same_lyr)
	{
		length0 = gridSize * sqrt(pow(i - i0, 2) + pow(j - j0, 2));
	}
	else
	{
		length0 = gridSize * sqrt(pow(i - i0, 2) + pow(j - j0, 2) + 1);
	}
	Vector3d pdist_v = (*layer1)[i][j].pos - (*layer0)[i0][j0].pos;
	return 1000 * (pdist_v.norm() - length0) * pdist_v / pdist_v.norm();
}

// Resets force each ball of both layer to (0,0,0)
void clearLayerForce(int i, int j, grid_unit (*layer1)[plateSize][plateSize], grid_unit (*layer2)[plateSize][plateSize])
{
	(*layer1)[i][j].f << 0, 0, 0;
	(*layer2)[i][j].f << 0, 0, 0;
}

// Update each ball's force in layer0, given the other layer's position.
// Can apply force to the same layer by giving the same pointer to both layer parameter.
void updateLayerForce(int i, int j, grid_unit (*layer1)[plateSize][plateSize], grid_unit (*layer0)[plateSize][plateSize], float gridSize)
{
	bool isSameLyr = (layer1 == layer0);

	if (!isSameLyr) // directly over / under
	{
		(*layer0)[i][j].f += spring_f(i, j, i, j, layer1, layer0, isSameLyr, gridSize);
	}
	if (i != 0 && j != (plateSize - 1)) // left up
	{
		(*layer0)[i][j].f += spring_f(i - 1, j + 1, i, j, layer1, layer0, isSameLyr, gridSize);
	}
	if (j != (plateSize - 1)) // up
	{
		(*layer0)[i][j].f += spring_f(i, j + 1, i, j, layer1, layer0, isSameLyr, gridSize);
	}
	if (i != (plateSize - 1) && j != (plateSize - 1)) // up right
	{
		(*layer0)[i][j].f += spring_f(i + 1, j + 1, i, j, layer1, layer0, isSameLyr, gridSize);
	}
	if (i != (plateSize - 1)) // right
	{
		(*layer0)[i][j].f += spring_f(i + 1, j, i, j, layer1, layer0, isSameLyr, gridSize);
	}
	if (i != (plateSize - 1) && j != 0) // right down
	{
		(*layer0)[i][j].f += spring_f(i + 1, j - 1, i, j, layer1, layer0, isSameLyr, gridSize);
	}
	if (j != 0) // down
	{
		(*layer0)[i][j].f += spring_f(i, j - 1, i, j, layer1, layer0, isSameLyr, gridSize);
	}
	if (i != 0 && j != 0) // down left
	{
		(*layer0)[i][j].f += spring_f(i - 1, j - 1, i, j, layer1, layer0, isSameLyr, gridSize);
	}
	if (i != 0) // left
	{
		(*layer0)[i][j].f += spring_f(i - 1, j, i, j, layer1, layer0, isSameLyr, gridSize);
	}
}

// Updates one layer's position and velocity.
void updateLayerPV(grid_unit (*layer)[plateSize][plateSize], float dt)
{
	for (int i = 0; i < plateSize; i++)
	{
		for (int j = 0; j < plateSize; j++)
		{
			(*layer)[i][j].v += (*layer)[i][j].f / (*layer)[i][j].m * dt;
			(*layer)[i][j].pos += (*layer)[i][j].v * dt;
		}
	}
}

// Linear mapping, I guess...
// from (pa,pb,x) to (qa,qb,y), outputs y
float map_to(float pa, float pb, float qa, float qb, float x)
{
	return qa + (qb-qa)* (pb-x)/(pb-pa);
}

// output a picture.
// mode 0 = b&w pic, 1 = red&blue
void outputPic(grid_unit (*layer)[plateSize][plateSize], int mode, bool writeFile, int frame)
{
	
	cv::Mat image(plateSize, plateSize, CV_8UC3);

	if (mode == 0)
	{
		for (int i = 0; i < plateSize; i++)
		{
			for (int j = 0; j < plateSize; j++)
			{
				float value = (*layer)[i][j].pos(1);
				cv::Vec3b &color = image.at<cv::Vec3b>(i, j);
				color[0] = map_to(-3, 3, 255, 0, value);
				color[1] = map_to(-3, 3, 255, 0, value);
				color[2] = map_to(-3, 3, 255, 0, value);
			}
		}
	}
	else
	{
		for (int i = 0; i < plateSize; i++)
		{
			for (int j = 0; j < plateSize; j++)
			{
				float value = (*layer)[i][j].pos(1);
				cv::Vec3b &color = image.at<cv::Vec3b>(i, j);

				if (value > 0)
				{
					color[0] = 0;
					color[1] = 0;
					// color[2] = map_to(0, 10, 255, 0, value);
					if ((int)(abs(value / 3) * 255) <= 255)
					{
						color[2] = (int)(abs(value / 3) * 255);
					}
					else
					{
						color[2] = 255;
					}
				}
				else
				{
					// color[0] = map_to(0, -10, 255, 0, value);
					if ((int)(abs(value / 3) * 255) <= 255)
					{
						color[0] = (int)(abs(value / 3) * 255);
					}
					else
					{
						color[0] = 255;
					}
					color[1] = 0;
					color[2] = 0;
				}
			}
		}
	}

	// cv::eigen2cv(output,output2);
	cv::namedWindow("Chladni", cv::WINDOW_AUTOSIZE);
	cv::resizeWindow("Chladni", 500,500);
	cv::imshow("Chladni", image);
	cv::waitKey(1);

	if(writeFile){
		char buf[50];
		sprintf(buf, "./pics/%04d.png", frame);
		cv::imwrite(buf, image);
	}
}

int main(int argc, char *argv[])
{
	grid_unit layer1[plateSize][plateSize];
	grid_unit layer2[plateSize][plateSize];
	float gridSize = 4; // distance between adjacent balls

	initialize_layer(gridSize, &layer1, 0);
	initialize_layer(gridSize, &layer2, gridSize);

	system("mkdir pics"); // create pics folder if not exist
	system("rm -f pics/*.png"); // clear the floder if it do exist

	// cout << spring_f(5, 5, 5, 6, &layer1, &layer1, true, gridSize) << endl;

	float t = 0;
	const float dt = 0.001;

	int frame = 0;

	float frequency = 2;
	float amplitude = 2.0;
	int test_x = 12;
	int test_y = 12; // vibrating point

	while (frame <= 2200) // main time loop
	{
		t += dt;
		frame += 1;

		if (!(frame % 10))
		{
			cout << "Writing frame " << frame << endl;
		}

		// cout << layer1[11][11].pos << endl;
		// cout << "Layer 2 "<< layer2[11][11].pos(1) << endl;

		for (int i = 0; i < plateSize; i++)
		{
			for (int j = 0; j < plateSize; j++)
			{
				clearLayerForce(i, j, &layer1, &layer2);

				updateLayerForce(i, j, &layer2, &layer1, gridSize);
				updateLayerForce(i, j, &layer1, &layer1, gridSize);
				updateLayerForce(i, j, &layer1, &layer2, gridSize);
				updateLayerForce(i, j, &layer2, &layer2, gridSize);
			}
		}

		updateLayerPV(&layer1, dt);
		updateLayerPV(&layer2, dt);

		// apply vibration
		layer1[test_x][test_y].pos << gridSize * test_x, amplitude * sin(t * 2 * M_PI * frequency), gridSize * test_y;
		layer2[test_x][test_y].pos << gridSize * test_x, amplitude * sin(t * 2 * M_PI * frequency) + gridSize, gridSize * test_y;

		if(!(frame % 10)){
			outputPic(&layer1,1,true,frame);
		}else{
			outputPic(&layer1,1,false,frame);
		}
	}
	cout << "Task done.\07" << endl; // ascii 07 rings a bell
	return 0;
}