#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <SDL.h>
#include <cmath>

struct Node //node struct used to represent each map position
{
	int x;
	int y;
	float cost;
	Node* previous = NULL; //pointer to the previous node that defaults to null
	Node(int _x, int _y, Node* _prev, int _startX, int _startY, int _endX, int _endY)
	{
		x = _x;
		y = _y;
		previous = _prev;
		cost = abs(x - _startX) + abs(x - _endX) + abs(y - _startY) + abs(y - _endY);
	}
	Node() {}
};

void createFinalList(Node* _node, std::vector<Node>& _list);

void drawWindow(SDL_Renderer* renderer, SDL_Window* window, std::vector< std::vector<int> > map, int x, int y, int currentX, int currentY);

int main(int argc, char* args[])
{
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		std::cout << " Failed to initialize SDL : " << SDL_GetError() << std::endl;
		return -1;
	}
	std::vector<Node*> openList;
	std::vector<Node*> closedList;
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
	for (int i = 0; i < y; i++) //input map
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
	Node* temp = new Node(startX, startY, NULL, startX, startY, endX, endY);
	openList.push_back(temp);
	double timeStart = clock();
	while (!openList.empty()) //continues algorithm until all squares have been checked or the optimal path has been found
	{
		Node* current = new Node(); //default current node
		float lowCost = 100000000; //set base cost to a very large number
		int lowest = -1;
		for (int i = 0; i < openList.size(); i++)
		{
			if (openList[i]->cost < lowCost) //selects the node with the lowest cost
			{
				lowCost = openList[i]->cost;
				lowest = i;
			}
		}
		std::vector<Node*>::iterator it = openList.begin() + lowest; //creates an iterator at the position of the chosen node
		current = openList[lowest]; //copies the chosen node in the current one being used
		openList.erase(it); //removes chosen node from open list
		closedList.push_back(current); //adds chosen node to closed list
		if ((current->x == endX) && (current->y == endY))
		{
			openList.clear(); //ends algorithm is optimal path is found
		}
		else
		{
			for (int j = current->x - 1; j < current->x + 2; j++)
			{
				for (int k = current->y - 1; k < current->y + 2; k++) //iterates through every x and y value around the current node
				{
					bool test = true; //test for if the node being tested is valid
					if ((j >= 0) && (k >= 0) && (j < x) && (k < y))
					{
						for (int q = 0; q < openList.size(); q++)
						{
							if ((openList[q]->x == j) && (openList[q]->y == k))
							{
								test = false; //checks open list for tested node
							}
						}
						for (int q = 0; q < closedList.size(); q++)
						{
							if ((closedList[q]->x == j) && (closedList[q]->y == k))
							{
								test = false; //checks closed list for tested node
							}
						}
						if (map[k][j] == 1)
						{
							test = false; //returns false if node is a wall
						}
						if (test)
						{
							Node* temp = new Node(j, k, current, startX, startY, endX, endY); //adds node to open list if valid
							openList.push_back(temp);
							temp = NULL;
							delete temp;
						}
					}
				}
			}
		}
		current = NULL;
		delete current; //clears current node pointer from memory
	}
	std::vector<Node> finalList;
	createFinalList(closedList.back(), finalList); //traces back closed list to find the final path
	double timeStop = clock();
	std::cout << "Time: " << (timeStop - timeStart) / double(CLOCKS_PER_SEC) * 1000 << std::endl; //outputs the time the algorithm took in ms
	SDL_Window *window;
	SDL_Renderer *renderer;
	window = SDL_CreateWindow("AI Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 1000, SDL_WINDOW_OPENGL);
	if (window == NULL)
	{
		std::cout << "Window error" << std::endl;
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	for (int q = 0; q < finalList.size(); q++) //shows graphically the path found by the algorithm
	{
		drawWindow(renderer, window, map, x, y, finalList[q].x, finalList[q].y);
		SDL_Delay(500);
	}
	std::cin.get();
	std::cin.get();
	return 0;
}

void createFinalList(Node* _node, std::vector<Node>& _list)
{
	if (_node->previous != NULL)
	{
		createFinalList(_node->previous, _list); //calls recursively if the node isn't the start node
	}
	_list.push_back(*_node); //pushes the current node onto the final list starting with the start node
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