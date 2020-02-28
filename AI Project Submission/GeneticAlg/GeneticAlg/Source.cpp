#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <SDL.h>
#include <cmath>

void drawWindow(SDL_Renderer* renderer, SDL_Window* window, std::vector< std::vector<int> > map, int x, int y, int currentX, int currentY);

double sigmoid(double _x);

int main(int argc, char* args[])
{
	
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		std::cout << " Failed to initialize SDL : " << SDL_GetError() << std::endl;
		return -1;
	}

	const char* filePtr = "Map3UTF_8.txt";
	std::ifstream file;
	file.open(filePtr, std::fstream::in);
	std::string test;
	int x;
	int y;
	std::getline(file, test);
	std::stringstream line(test);
	line >> x;
	std::getline(file, test);
	line >> y;
	std::vector< std::vector<int> > map(y, std::vector<int>(x, 0));
	int startX, startY, endX, endY; //saves the start and end positions in the map
	for (int i = 0; i < y; i++) //input map from the string stream
	{
		for (int j = 0; j < x; j++)
		{
			std::getline(file, test);
			line >> map[i][j];
			if (map[i][j] == 2)
			{
				startX = j;
				startY = i;
			}
			if (map[i][j] == 3)
			{
				endX = j;
				endY = i;
			}
		}
	}
	file.close();

	int pop = 1000; //the number of individuals
	int length = 20; //the length of the chromosomes
	std::vector< std::vector<int> > chromo(pop, std::vector<int>(length, 0)); //initialises the 2D vector for the chromosomes
	srand(time(NULL)); //seeds the randomiser with the current time
	std::vector<double> fitness(pop, 0.0); //initialises a vector for the fitness of each individual
	double highestFitness = 0; //tracks the highest fitness
	double AvFitness = 0;
	double timeStart = clock(); //checks the clock before the start of the algorithm
	for (int i = 0; i < pop; i++) //initialise chromosomes
	{
		for (int j = 0; j < length; j++)
		{
			int r = rand();
			if (r < RAND_MAX / 2)
			{
				chromo[i][j] = 1;
			}
			else
			{
				chromo[i][j] = 0;
			}
		}
	}
	for (int i = 0; i < pop; i++) //prints starting chromosomes
	{
		for (int j = 0; j < length; j++)
		{
			std::cout << chromo[i][j];
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
	for (int loop = 0; loop < 1000; loop++)
	{
		int position; //position in the chromosome
		int currentX, currentY; //current position of individual on the map
		for (int i = 0; i < pop; i++)
		{
			fitness[i] = 0; //resets fitness to 0
		}
		int wallHits; //tracks the number of times an individual hits a wall
		for (int i = 0; i < pop; i++) //calculates the fitness of each individual
		{
			position = 0;
			wallHits = 0;
			currentX = startX;
			currentY = startY;
			while (position < length)
			{
				if (chromo[i][position] == 0)
				{
					if (chromo[i][position + 1] == 0)
					{
						currentY -= 1;
						if (currentY < 0)
						{
							currentY += 1;
							wallHits += 1;
						}
						else if (map[currentY][currentX] == 1)
						{
							currentY += 1;
							wallHits += 1;
						}
					}
					if (chromo[i][position + 1] == 1)
					{
						currentX += 1;
						if (currentX >= x)
						{
							currentX -= 1;
							wallHits += 1;
						}
						else if (map[currentY][currentX] == 1)
						{
							currentX -= 1;
							wallHits += 1;
						}
					}
				}
				if (chromo[i][position] == 1)
				{
					if (chromo[i][position + 1] == 0)
					{
						currentY += 1;
						if (currentY >= y)
						{
							currentY -= 1;
							wallHits += 1;
						}
						else if (map[currentY][currentX] == 1)
						{
							currentY -= 1;
							wallHits += 1;
						}
					}
					if (chromo[i][position + 1] == 1)
					{
						currentX -= 1;
						if (currentX < 0)
						{
							currentX += 1;
							wallHits += 1;
						}
						else if (map[currentY][currentX] == 1)
						{
							currentX += 1;
							wallHits += 1;
						}
					}
				}
				/*if (map[currentY][currentX] == 3)
				{
					length = position;
					for (int j = 0; j < pop; j++)
					{
						chromo[j].resize(length + 2);
					}
				}*/
				position += 2;
			}
			double t = exp(-(((double)abs(currentX - startX) + (double)abs(currentY - startY) + (length - position)) - (((double)abs(currentX - endX) + (double)abs(currentY - endY) + wallHits))));
			fitness[i] = (1.0 / (t + 1.0)); //fitness function is sigmoid where x = a set of variables created by the fitness test
		}
		double totalFitness = 0;
		for (int i = 0; i < pop; i++) //calculates the total and highest fitness
		{
			if (fitness[i] > 0.85)
			{
				totalFitness += fitness[i];
			}
			if (fitness[i] > highestFitness)
			{
				highestFitness = fitness[i];
				std::cout << loop << " " << highestFitness << std::endl;
			}
		}
		AvFitness = totalFitness / pop;
		if (loop % 100 == 0) //outputs the average fitness every 100 iterations
		{
			std::cout << "Average of " << loop << " " << AvFitness << std::endl;
		}
		if ((highestFitness < (1.0 / (exp(-(((double)abs(endX - startX) + (double)abs(endY - startY)))) + 1.0)) && (loop < 1000))) 
			//this calculates the fitness if the individual reaches the goal without hitting a wall and stops the algorithm if this has been achieved
		{
			std::vector<int> pairs(pop, 0);
			int eliteTest = 0;
			for (int i = 0; i < pop; i++) //creates a set of pairs to create the next generation
			{
				double indiv = rand() % 100;
				double test = 0;
				for (int j = 0; j < pop; j++)
				{
					if (fitness[i] > 0.85)
					{
						test += (fitness[j] / totalFitness) * 100.0f; //calculates chance of each individual breeding
						if ((fitness[j] == highestFitness) && (eliteTest < 2)) //forces an individual with the highest fitness to breed with itself
						{
							pairs[0] = j;
							pairs[1] = j;
							i = 2;
							j = pop;
							eliteTest += 2;
						}
						else if (indiv < test)
						{
							pairs[i] = j;
							j = pop;
						}
					}
					else if (totalFitness == 0)
					{
						pairs[i] = rand() % pop;
					}
				}
			}
			for (int a = 0; a < pop; a += 2) //crossover algorithm
			{
				float cross = (float)rand() / (float)RAND_MAX;
				if (cross < 0.7f)
				{

					int start = rand() % length;
					int end = rand() % (length - start); //randomises position of crossover
					std::vector< std::vector<int> > temp(2, std::vector<int>(start + end, 0));
					for (int k = start; k < (start + end); k++)
					{
						temp[0][(k - start)] = chromo[pairs[a]][k];
						temp[1][(k - start)] = chromo[pairs[(a + 1)]][k];
					}
					for (int j = start; j < (start + end); j++)
					{
						chromo[pairs[a]][j] = temp[1][j - start];
						chromo[pairs[a + 1]][j] = temp[0][j - start];
					}
				}

			}
			for (int i = 0; i < pop; i++) //mutation algorithm
			{
				for (int j = 0; j < length; j++)
				{
					float mutate = (float)rand() / (float)RAND_MAX;
					if (mutate < 0.001)
					{
						if (chromo[i][j] == 0)
						{
							chromo[i][j] = 1;
						}
						else
						{
							chromo[i][j] = 0;
						}
					}
				}
			}
		}
		else
		{
			loop = 1000; //ends the algorithm if task is completely satifactorily
		}
	}
	double timeStop = clock(); //checks the clock at the end of the algorithm
	for (int i = 0; i < pop; i++)
	{
		if (fitness[i] > 0.7) //outputs the chromosomes with satisfactory fitness
		{
			for (int j = 0; j < length; j++)
			{
				std::cout << chromo[i][j];
			}
			std::cout << std::endl;
		}
	}

	SDL_Window *window;
	SDL_Renderer *renderer;
	window = SDL_CreateWindow("AI Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 1000, SDL_WINDOW_OPENGL);
	if (window == NULL)
	{
		std::cout << "Window error" << std::endl;
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); //sets up game window and renderer
	int currentX, currentY;
	for (int i = 0; i < pop; i++) //uses sdl to show the path of the top individuals
	{
		if (fitness[i] > 0.7) //only shows individuals with a fitness worth seeing
		{
			int position = 0;
			currentX = startX;
			currentY = startY;
			drawWindow(renderer, window, map, x, y, currentX, currentY);
			std::cout << i << " " << fitness[i] << std::endl;
			SDL_Delay(500);
			while (position < length)
			{
				if (chromo[i][position] == 0)
				{
					if (chromo[i][position + 1] == 0)
					{
						currentY -= 1;
						if (currentY < 0)
						{
							currentY += 1;
							std::cout << "wall hit" << std::endl;
						}
						else if (map[currentY][currentX] == 1)
						{
							currentY += 1;
							std::cout << "wall hit" << std::endl;
						}
					}
					if (chromo[i][position + 1] == 1)
					{
						currentX += 1;
						if (currentX >= x)
						{
							currentX -= 1;
							std::cout << "wall hit" << std::endl;
						}
						else if (map[currentY][currentX] == 1)
						{
							currentX -= 1;
							std::cout << "wall hit" << std::endl;
						}
					}
				}
				if (chromo[i][position] == 1)
				{
					if (chromo[i][position + 1] == 0)
					{
						currentY += 1;
						if (currentY >= y)
						{
							currentY -= 1;
							std::cout << "wall hit" << std::endl;
						}
						else if (map[currentY][currentX] == 1)
						{
							currentY -= 1;
							std::cout << "wall hit" << std::endl;
						}
					}
					if (chromo[i][position + 1] == 1)
					{
						currentX -= 1;
						if (currentX < 0)
						{
							currentX += 1;
							std::cout << "wall hit" << std::endl;
						}
						else if (map[currentY][currentX] == 1)
						{
							currentX += 1;
							std::cout << "wall hit" << std::endl;
						}
					}
				}
				if (map[currentY][currentX] == 3)
				{
					position = length;
				}
				drawWindow(renderer, window, map, x, y, currentX, currentY);
				SDL_Delay(500);
				position += 2;
			}
		}
	}
	std::cout << "finished" << std::endl;
	std::cout << "Time: " << (timeStop - timeStart) / double(CLOCKS_PER_SEC) * 1000 << std::endl; //outputs the time the algorithm took in ms
	std::cin.get();
	std::cin.get();
	SDL_Quit();
	return 0;
}

void drawWindow(SDL_Renderer* renderer, SDL_Window* window, std::vector< std::vector<int> > map, int x, int y, int currentX, int currentY)
{
	SDL_RenderClear(renderer);
	SDL_Rect square;
	SDL_Surface* surface;
	for (int j = 0; j < y; j++)
	{
		for (int k = 0; k < x; k++)
		{
			surface = SDL_CreateRGBSurface(0, 1000, 1000, 32, 0, 0, 0, 0);
			square.x = k * 100 + ((k + 1) * 5);
			square.y = j * 100 + ((j + 1) * 5);
			square.w = 100;
			square.h = 100;
			if (map[j][k] == 0)
			{
				SDL_FillRect(surface, &square, SDL_MapRGB(surface->format, 255, 255, 255));
			}
			if (map[j][k] == 1)
			{
				SDL_FillRect(surface, &square, SDL_MapRGB(surface->format, 0, 0, 0));
			}
			if (map[j][k] == 2)
			{
				SDL_FillRect(surface, &square, SDL_MapRGB(surface->format, 0, 255, 0));
			}
			if (map[j][k] == 3)
			{
				SDL_FillRect(surface, &square, SDL_MapRGB(surface->format, 0, 0, 255));
			}
			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
			SDL_FreeSurface(surface);
			SDL_RenderCopy(renderer, texture, NULL, &square);
			SDL_DestroyTexture(texture);
		}
	}
	surface = SDL_CreateRGBSurface(0, 1000, 1000, 32, 0, 0, 0, 0);
	square.x = (currentX * 100) + ((currentX + 1) * 5);
	square.y = (currentY * 100) + ((currentY + 1) * 5);
	square.w = 100;
	square.h = 100;
	SDL_FillRect(surface, &square, SDL_MapRGB(surface->format, 255, 0, 0));
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	SDL_RenderCopy(renderer, texture, NULL, &square);
	SDL_DestroyTexture(texture);
	SDL_RenderPresent(renderer);
}

double sigmoid(double _x)
{
	return 1.0 / (1.0 + exp(-_x));
}