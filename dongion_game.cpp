// dongion_game.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h> 
#include <algorithm> //heap

//#include "dungeonHeader.h"

using namespace std;
const int NY = 160;//80;//160; //width
const int NX = 96;// 48;//96; //height
//terrain[height][width] //terrain[NX][NY]
const int numb_of_rooms = 12;//12;  //in class dungeon_map private
const int room_min_y = 8;
const int room_min_x = 5;
const int room_seperation=3; //hausdoff_distance = 3;
//size of rooms is in place

class room_class{
	int upper_left_y;
	int upper_left_x;
	int width;
	int height;
	int placed; //if room is placed change to 0
	int connected; //change to 0 if connected;
public:
	room_class()
	{
		placed=1; //if room is placed change to 0
		connected=1; //change to 0 if connected;
	}
	int get_upper_left_y()
	{
		return upper_left_y;
	}
	int get_upper_left_x()
	{
		return upper_left_x;
	}
	int get_width()
	{
		return width;
	}
	int get_height()
	{
		return height;
	}
	void set_upper_left_y(int value)
	{
		upper_left_y = value;
	}
	void set_upper_left_x(int value)
	{
		upper_left_x = value;
	}
	void set_width(int value)
	{
		width = value;
	}
	void set_height(int value)
	{
		height = value;
	}
	void is_placed()
	{
		placed = 0;
	}
};

class dungeon_map{
private:
	char terrain[NX][NY]; //whats shown on screen
	char creature_placement[NX][NY]; //where monsters and pc and npcs are
	//room_class room_list[numb_of_rooms];
	char cell_data[NX][NY]; // data for each cell, ex: stair, wall, room, edge, coordor
	int hardness[NX][NY];
	// w=wall, i= edge, r= room, c=coordors

	void blank_dungeon()
	{
		int i, j, y ,m;
		for (i = 0; i < NX; i++)
		{
			for (j = 0; j < NY; j++)
			{
				terrain[i][j] = '#';
				cell_data[i][j] = 'w';
			}
		}
		
		for (y = 0; y < NX; y++)
		{
			cell_data[y][0] = 'i';
			cell_data[y][NY-1] = 'i';
		}
		for (m = 0; m < NY; m++)
		{
			cell_data[0][m] = 'i';
			cell_data[NX-1][m] = 'i';
		}
		
		return;
	}
	void make_rooms(room_class &room_list)
	{
		room_class *room_list2 = &room_list;
		int timesFailedPlace = 0;
		int return_BuildRooms = 0;
		int place_attempts = 0; //number of attepts to place each room
		int room_index_number = 0;
		while((room_index_number<numb_of_rooms) && (place_attempts<2000))
		{
			//change visabliity //done
			//room_class *test_room;
			//test_room = new room_class[numb_of_rooms]; // = room_list;
			return_BuildRooms = place_rooms(room_index_number, room_list2[room_index_number]);//<<<<<problem here
			//return_BuildRooms = 999;
			if (return_BuildRooms == 2) //falure to place room
			{
				place_attempts++;
				//room_index_number++;
			}
			else if (return_BuildRooms == 1) //placed room
			{
				room_index_number++;
				place_attempts = 0;
			}
			else
			{
				place_attempts = 999;
				//room_index_number++;
			}
		}
		//place_stairs(room_list);
		return;
	}
	int place_rooms(int room_index_number, room_class &the_room)
	{
		room_class *room_list2 = &the_room;
		//if (percentOpen>(openArea / sizeArea))  //should check to see if dungion is too full alrready // change to make_rooms
				//return 0;
		//for upper left hand corner, random from 3 to 3 less than N(y/x)
		int randY = (rand() % (NY-3))+3;
		int randX = (rand() % (NX - 3)) + 3;
		//size of room (y: 8-> 18, dif 10; x: 5->15, dif 10
		int randYdist = rand() % 20 + 8;
		int randXdist = rand() % 20 + 5; 

		int roomOK = safe_room(randY, randX, randYdist, randXdist); //check to see if can place room there
		if (roomOK == 0) //placed
		{	//turn room area of terrain to "." (period)
			int y_lower_right = randYdist + randY;
			int x_lower_right = randXdist + randX;
			int i, j;
			for (i = randY; i <= y_lower_right; i++)
			{
				for (j = randX; j <= x_lower_right; j++)
				{
					terrain[j][i] = '.';
					cell_data[j][i] = 'r';
				}
			}
			//add elements to room
			the_room.set_upper_left_y(randY);
			the_room.set_upper_left_x(randX);
			the_room.set_width(randXdist);
			the_room.set_height(randYdist);
			the_room.is_placed();
			//openArea = (randk*randX) + openArea; //to make so only certen amount of dungion is full
			return 1;
		}
		else //failed to place
		{
			return 2;
		}
	}
	int safe_room(int randY, int randX, int randYdist, int randXdist)
	{
		int y_lower_right = randYdist + randY;
		int x_lower_right = randXdist + randX;
		// 3 spaces from left, top, right, bottom
		if ((randY < 3) || (randX < 3) || ((y_lower_right+3) >NY) || ((x_lower_right+3) >NY) )
		{
			return 1;
		}
		else
		{
			//check to see if area is occupied and to close to another room by
			//		by seeing is there is open space withen sq 3 biger each way of this.room
			int isOccupied=0;
			int notOccupied = 0;
			int i;
			int j;
			for (i = randY - 2; i <= y_lower_right + 2; i++)
			{
				for (j = randX - 2; j <= x_lower_right + 2; j++)
				{
					if (terrain[j][i] != '#')
					{
						notOccupied = 1;
					}
				}
			}
			if (notOccupied == 1) //area is occupied
			{
				return 5;
			}
			else
			{
				return 0;
			}
		}
	}

	//added from Obberton13  //when hes functions want x my rooms need to return y //currently changing
	void connect_all_rooms(room_class &room_list)
	{
		room_class *room_list2 = &room_list;
		int i;
		int connected[numb_of_rooms];
		for (i = 0; i<numb_of_rooms; i++)
		{
			connected[i] = 0;
		}
		connected[0] = 1;
		for (i = 0; i<numb_of_rooms; i++)
		{
			cout << "new room" << endl;
			cout << i << endl;
			//get index of clossest connected and unconnected rooms
			//find_closest_room(the room in question, array to tell is connected, unconected(0)/connected(1), *room_list2);
			//will return -1 for none, else index
			int closest_unconnected=find_closest_room(i, *connected, 0, *room_list2);
			int closest_connected=find_closest_room(i, *connected, 1, *room_list2); //will be -1 for first room
			cout << closest_unconnected << endl;
			cout << closest_connected << endl;

			if(closest_unconnected !=-1)
			{
				cout << "soomthing1.0 " << endl;
				connect_room(room_list2[i], room_list2[closest_unconnected]);
				connected[closest_unconnected] = 1;
				cout << "soomthing1.1 " << endl;
			}
			else if(closest_connected!=-1) //connect to clossest //done secound 
			{
				cout << "soomthing2.0" << endl;

				connect_room(room_list2[i], room_list2[closest_connected]);
				connected[closest_connected] = 1;// should not be needed
				cout << "soomthing2.1 " << endl;
			}
			else
			{
				cout << "soomthing went very wroung connecting the rooms in initialization" << endl;
			}
			
			
			//changing
			/*
			int closest = find_closest_room(i, connected, 1, *room_list2);
			if (closest == -1)
			{
				continue;
			}
			int closestConnected = find_closest_room(closest, connected, 0, *room_list2);
			if (closestConnected == -1)
			{
				continue;
			}
			//connecting closses connected and non-connected rooms but not room[i]
			connect_room(room_list2[closest], room_list2[closestConnected]); 
			connected[closest] = 1;
			*/
		}
	}

	int find_closest_room(int i, int &connected, int connected_unconnected, room_class &room_list)
	{
		//find_closest_room(the room in question, array to tell is connected, unconected(0)/connected(1), *room_list2);
		//cout << "in find_closest_room" << endl;
		//int connected[numb_of_rooms];
		int *connected_list=&connected;
		//unconected(0)/connected(1)
		room_class *room_list2 = &room_list;
		//int y; ///
		int j;
		int closest = -1;
		int minDist = INT_MAX;
		int cY = room_list2[i].get_upper_left_y() + (room_list2[i].get_width() / 2);
		int cX = room_list2[i].get_upper_left_x() + (room_list2[i].get_height() / 2);
		//cout << "size room list" << endl;
		//cout << sizeof(room_list2) << endl;
		for(j=0; j<numb_of_rooms; j++)
		{
			//cout << "in find_closest_room for loop" << endl;
			//cout << j << endl;
			if(connected_list[j] != connected_unconnected)//so it will ignore the connected/notconnected rooms
			{
				//do nothing
			}
			else
			{
				if(i==j)//are same index in room_list2
				{
					//do nothing
				}
				else
				{
					int centerX = room_list2[j].get_upper_left_y() + (room_list2[j].get_width() / 2);
					int centerY = room_list2[j].get_upper_left_x() + (room_list2[j].get_height() / 2);
					int dy = centerY - cY;
					int dx = centerX - cX;
					int dist = ((dy*dy) + (dx*dx)); //technically sqr root, but that doesn't really matter
					if (dist<minDist)
					{
						closest = j;
						minDist = dist;
					}
				}
			}
		}
		//cout << "closist is" << endl;
		//cout << closest << endl;
		return closest;

		//change //also x& y are swaped
		/*
		for (x = sizeof(room_list2) - 1; x >= 0; x--)
		{
			if (connected_list[x] != connected_unconnected)
			{
				continue;
			}
			int centerY = room_list2[x].get_upper_left_x() + (room_list2[x].get_width() / 2);
			int centerX = room_list2[x].get_upper_left_y() + (room_list2[x].get_height() / 2);
			int dx = centerX - cX;
			int dy = centerY - cY;
			int dist = ((dx*dx) + (dy*dy)); //technically sqr root, but that doesn't really matter
			if (dist<minDist)
			{
				closest = x;
				minDist = dist;
			}
		}
		return closest;
		*/

	}

	void connect_room(room_class &room1, room_class &room2)
	{ //neeeds work
		int p,w;
		/*
		for (p = 0; p < NX; p++)
		{
			for (w = 0; w < NY; w++)
			{
				//cout << "Hello world" << endl;
				cout << terrain[p][w];
			}
			cout << endl;
		}
		*/


		//room_class *room_numb_1 = &room1;
		//room_class *room_numb_2 = &room2;
		//not edditing room
		//int y = room1.get_upper_left_y() + (room1.get_width() / 2);
		//int x = room1.get_upper_left_x() + (room1.get_height() / 2);
		
		//centeer of rooms
		//putting these super negative
		int y_1=room1.get_upper_left_y() + (room1.get_width() / 2);
		int x_1=room1.get_upper_left_x() + (room1.get_height() / 2);

		int y_2=room2.get_upper_left_y() + (room2.get_width() / 2);
		int x_2=room2.get_upper_left_x() + (room2.get_height() / 2);

		int i, j;

		if(y_1<y_2)
		{
			for(i=y_1; i<=y_2; i++)
			{//make corrdor at terrain[x_1][i]
				//if(cell_data[x_1][i] != 'r')//cell_data[y][x]
				{
					cell_data[x_1][i] = 'c';
					terrain[x_1][i] = '.';
				}
			}
			if(x_1<x_2)
			{
				for(j=x_1; j<=x_2;j++)
				{//make corrdor at terrain[j][i]
					//if(cell_data[j][y_2] != 'r')
					{
						cell_data[j][y_2] = 'c';
						terrain[j][y_2] = '.';
					}
				}
			}
			else
			{
				for(j=x_2; j<=x_1;j++)
				{ //make corrdor at terrain[j][i]
					//if(cell_data[j][y_2] != 'r')
					{
						cell_data[j][y_2] = 'c';
						terrain[j][y_2] = '.';
					}
				}
			}
		}
		else
		{
			for(i=y_2; i<=y_1; i++)
			{//make corrdor at terrain[y_1][i]
				//if(cell_data[x_1][i] != 'r')
				{
					cell_data[x_1][i] = 'c';
					terrain[x_1][i] = '.';
				}
			}
			if(x_1<x_2)
			{
				for(j=x_1; j<=x_2;j++)
				{ //make corrdor at terrain[j][i]
					//if(cell_data[j][y_1] != 'r')
					{
						cell_data[j][y_1] = 'c';
						terrain[j][y_1] = '.';
					}
				}
			}
			else
			{
				for(j=x_2; j<=x_1;j++)
				{ //make corrdor at terrain[j][i]
					//if(cell_data[j][y_1] != 'r')
					{
						cell_data[j][y_1] = 'c';
						terrain[j][y_1] = '.';
					}
				}
			}
		}
		
		//change x and y
		/*
		while (!inRoom(x, y, room1))//
		{
			if (x<room2.get_upper_left_x() + (room2.get_width() / 2))
			{
				x++;
			}
			else if (x>room2.get_upper_left_x() + (room2.get_width() / 2))
			{
				x--;
			}
			if (y<room2.get_upper_left_y() + (room2.get_height() / 2))
			{
				y++;
			}
			else if (y>room2.get_upper_left_y() + (room2.get_height() / 2))
			{
				y--;
			}
		}
		while (inRoom(x, y, room2))
		{
			int i;
			int d = rand()  % 4 + 2;
			if (x<room2.get_upper_left_x() + (room2.get_width() / 2))
			{
				for (i = 0; i<d; i++)
				{
					x++;
					//flip x and y
					if (cell_data[y][x] != 'r')
					{
						cell_data[y][x]  = 'c';
						terrain[y][x]  = '.';
					}
				}
			}
			else if (x>room2.get_upper_left_x() + (room2.get_width() / 2))
			{
				for (i = 0; i<d; i++)
				{
					--x;
					if (cell_data[y][x]  != 'r')
					{
						cell_data[y][x]  = 'c';
						terrain[y][x]  = '.';
					}
				}
			}
			else
			{
				int r = rand() % 3;
				d /= 2;
				switch (r)
				{
				case 1:
					for (i = 0; i<d; i++)
					{
						++x;
						if (cell_data[y][x]  != 'r')
						{
							cell_data[y][x]  = 'c';
							terrain[y][x]  = '.';
						}
					}
					break;
				case 2:
					for (i = 0; i<d; i++)
					{
						--x;
						if (cell_data[y][x]  != 'r')
						{
							cell_data[y][x]  = 'c';
							terrain[y][x]  = '.';
						}
					}
					break;
				default:
					break;
				}
			}
			d = rand() % 4 + 2;
			if (y<room2.get_upper_left_y() + (room2.get_height() / 2))
			{
				for (i = 0; i<d; i++)
				{
					++y;
					if (cell_data[y][x]  != 'r')
					{
						cell_data[y][x]  = 'c';
						terrain[y][x]  = '.';
					}
				}
			}
			else if (y>room2.get_upper_left_y() + (room2.get_height() / 2))
			{
				for (i = 0; i<d; i++)
				{
					--y;
					if (cell_data[y][x]  != 'r')
					{
						cell_data[y][x]  = 'c';
						terrain[y][x]  = '.';
					}
				}
			}
			else
			{
				int r = rand() % 3;
				d /= 2;
				switch (r)
				{
				case 1:
					for (i = 0; i<d; i++)
					{
						++y;
						if (cell_data[y][x]  != 'r')
						{
							cell_data[y][x]  = 'c';
							terrain[y][x]  = '.';
						}
					}
					break;
				case 2:
					for (i = 0; i<d; i++)
					{
						--y;
						if (cell_data[y][x]  != 'r')
						{
							cell_data[y][x]  = 'c';
							terrain[y][x]  = '.';
						}
					}
					break;
				default:
					break;
				}
			}
		}
		*/
		
	}

	int inRoom(int y, int x, room_class room) //in a specific room
	{
		if (y <= room.get_upper_left_y() || y >= room.get_upper_left_y() + room.get_width() - 1 || x <= room.get_upper_left_x() || x >= room.get_upper_left_x() + room.get_height() - 1)
		{
			return 1;
		}
		return 0;
	}


	//swap x&y
	/*
	int connect_rooms(room_class room_list[numb_of_rooms])
	{
		int i;
		for (i = 1; i < numb_of_rooms; i++) 
		{
			//connect_two_rooms(room i-1, room + i);
			dijkstra_corridor(room_list[i - 1].get_upper_left_x(), room_list[i - 1].get_upper_left_y(), room_list[i].get_upper_left_x(), room_list[i].get_upper_left_y());
		}
		return 0;
	}
	//connect two rooms gets rand point in each room than calles dijkstra_corridor, ill use uper left corners of each room
	void dijkstra_corridor(int room_1_X, int room_1_Y, int room_2_X, int room_2_Y)
	{

		typedef struct corridor_path {
		heap_node_t *hn;
		int pos[2];
		int pos_x[2];
		int pos_y[2];
		int from[2];
		int from_x[2];
		int from_y[2]
		//Because paths can meander about the dungeon, they can be 
		//significantly longer than DUNGEON_X.                     
		int cost;
		} corridor_path_t;

		//static void dijkstra_corridor(dungeon_t *d, pair_t from, pair_t to)



		static corridor_path_t path[NY][NX], *p;
		static int initialized = 0;
		heap_t h;
		int x, y;;
		//initalize 
		for (y = 0; y < NY; y++) {
			for (x = 0; x < NX; x++) 
			{
				path[y][x].pos[dim_y] = y;
				path[y][x].pos[dim_x] = x;
			}
		}
		//set distance to infinity
		for (y = 0; y < NY; y++) {
			for (x = 0; x < NX; x++) {
				path[y][x].cost = INT_MAX;
			}
		}

		path[from[dim_y]][from[dim_x]].cost = 0;

		heap_init(&h, corridor_path_cmp, NULL);

		for (y = 0; y < NY; y++) {
			for (x = 0; x < NX; x++) {
				if (cell_data[y][x] != 'i') {
					path[y][x].hn = heap_insert(&h, &path[y][x]);
				}
				else {
					path[y][x].hn = NULL;
				}
			}
		}

		while ((p = heap_remove_min(&h))) {
			p->hn = NULL;

			if ((p->pos[dim_y] == room_2_Y) && p->pos[dim_x] == room_2_X)
			{
				for (x = room_2_X, y = room_2_Y;
					(x != from[dim_x]) || (y != from[dim_y]);
					p = &path[y][x], x = p->from[dim_x], y = p->from[dim_y]) 
				{
					if (cell_data[y][x] != 'i') 
					{
						cell_data[y][x] = 'c';
					}
				}
				heap_delete(&h);
				return;
			}
			if ((path[p->pos[dim_y] - 1][p->pos[dim_x]].hn) && (path[p->pos[dim_y] - 1][p->pos[dim_x]].cost > 
				p->cost + (hardnesspair(p->pos) ? hardnesspair(p->pos) : 8) + ((p->pos[dim_x] != from[dim_x]) ? 48 : 0))) 
			{
				path[p->pos[dim_y] - 1][p->pos[dim_x]].cost = p->cost + (hardnesspair(p->pos) ? hardnesspair(p->pos) : 8) + ((p->pos[dim_x] != from[dim_x]) ? 48 : 0);
				path[p->pos[dim_y] - 1][p->pos[dim_x]].from[dim_y] = p->pos[dim_y];
				path[p->pos[dim_y] - 1][p->pos[dim_x]].from[dim_x] = p->pos[dim_x];
				heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1][p->pos[dim_x]].hn);
			}
			if ((path[p->pos[dim_y]][p->pos[dim_x] - 1].hn) && (path[p->pos[dim_y]][p->pos[dim_x] - 1].cost >
				p->cost + (hardnesspair(p->pos) ? hardnesspair(p->pos) : 8) + ((p->pos[dim_y] != from[dim_y]) ? 48 : 0)))
			{
				path[p->pos[dim_y]][p->pos[dim_x] - 1].cost = p->cost + (hardnesspair(p->pos) ? hardnesspair(p->pos) : 8) + ((p->pos[dim_y] != from[dim_y]) ? 48 : 0);
				path[p->pos[dim_y]][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
				path[p->pos[dim_y]][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
				heap_decrease_key_no_replace(&h, path[p->pos[dim_y]][p->pos[dim_x] - 1].hn);
			}
			if ((path[p->pos[dim_y]][p->pos[dim_x] + 1].hn) &&(path[p->pos[dim_y]][p->pos[dim_x] + 1].cost >
				p->cost + (hardnesspair(p->pos) ? hardnesspair(p->pos) : 8) + ((p->pos[dim_y] != from[dim_y]) ? 48 : 0))) 
			{
				path[p->pos[dim_y]][p->pos[dim_x] + 1].cost = p->cost + (hardnesspair(p->pos) ? hardnesspair(p->pos) : 8) + ((p->pos[dim_y] != from[dim_y]) ? 48 : 0);
				path[p->pos[dim_y]][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
				path[p->pos[dim_y]][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
				heap_decrease_key_no_replace(&h, path[p->pos[dim_y]][p->pos[dim_x] + 1].hn);
			}
			if ((path[p->pos[dim_y] + 1][p->pos[dim_x]].hn) &&(path[p->pos[dim_y] + 1][p->pos[dim_x]].cost >
				p->cost + (hardnesspair(p->pos) ? hardnesspair(p->pos) : 8) +((p->pos[dim_x] != from[dim_x]) ? 48 : 0))) 
			{
				path[p->pos[dim_y] + 1][p->pos[dim_x]].cost =p->cost + (hardnesspair(p->pos) ? hardnesspair(p->pos) : 8) +((p->pos[dim_x] != from[dim_x]) ? 48 : 0);
				path[p->pos[dim_y] + 1][p->pos[dim_x]].from[dim_y] = p->pos[dim_y];
				path[p->pos[dim_y] + 1][p->pos[dim_x]].from[dim_x] = p->pos[dim_x];
				heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1][p->pos[dim_x]].hn);
			}
		}
	}
	*/
/*
	void place_stairs(room_class room_list[numb_of_rooms])
	{
		int i;
		for (i = 0; i < numb_of_rooms; i + 4)
		{
			//up stairs
			//NX//width
			//NY//height
			//  < is down
			//  > is up
			int upperleft_x = room_list[i].get_upper_left_x();
			int upperleft_y = room_list[i].get_upper_left_y();
			int width = room_list[i].get_width();
			int height = room_list[i].get_height();
			int mid_x = upperleft_x + (width / 2);
			int mid_y = upperleft_y + (height / 2);
			terrain[mid_y][mid_x]='>';
			cell_data[mid_y][mid_x]='U';
		}

		for (i = 2; i < numb_of_rooms; i + 4)
		{
			//down stairs
			//NX//width
			//NY//height
			//  < is down
			//  > is up
			int upperleft_x = room_list[i].get_upper_left_x();
			int upperleft_y = room_list[i].get_upper_left_y();
			int width = room_list[i].get_width();
			int height = room_list[i].get_height();
			int mid_x = upperleft_x + (width / 2);
			int mid_y = upperleft_y + (height / 2);
			terrain[mid_y][mid_x] = '<';
			cell_data[mid_y][mid_x]='D';
		}
	}
	*/
public:
	void gen_dungeon(room_class &room_list)
	{
		room_class *room_list2 = &room_list;
		//room_list = room_list; // = room_list;
		//return_BuildRooms = place_rooms(room_index_number, test_room[room_index_number]);//<<<<<problem here
		//room_class room_list[numb_of_rooms];
		//initalize set of rooms
		blank_dungeon();
		make_rooms(room_list);  //and place stairs
			//place_rooms(d);
			//safe_rooms(d)
		connect_all_rooms(room_list);
		//connect_rooms(room_list);

		//int help = room_list2[1].get_height();
		//place_stairs(room_list);
		//edge
		cout << "gen_dung end" << endl;
		return;
	}

	//just imported
	void load_dungeon()
{
	//need to add dungeonMap, numOfRooms, Rooms listOfRoomInfo[]
	
	int i,j; //numOfRooms;
	char markerString[5]={0x52,0x4c,0x47,0x32,0x32}; //to checck if file has correct name is hex for RLG229
	unsigned char FileMarkerbuffer[5];
	unsigned char FileVersionbuffer[4];
	unsigned char FileSizebuffer[4];
	unsigned char Cellbuffer[4];
	unsigned char NumberOfRoomsbuffer[2];
	unsigned char PosOfRoomsbuffer[4];
	
	int isMarkerCorrect=0;
	
	FILE *ptr;
	ptr=fopen("dungeon.0", "rb"); //r for read, b for binary //for very local file
	//ptr=fopen("$HOME/.rlg229/dungeon", "rb"); //for hand in
	//fread read in binary
	//fwrite write into binary heapp
	
	//name file "$HOME/.rlg229/dungeon"

	fread(FileMarkerbuffer,sizeof(FileMarkerbuffer),1,ptr); // read sizeof(FileMarkerbuffer) bytes to our buffer
	//each next fread reads next part of file, 
	
	//check if file is correct
	for(i = 0; i<sizeof(FileMarkerbuffer); i++)
	{
		//printf("%x ", FileMarkerbuffer[i]); // prints a series of bytes
		//printf("%x ", markerString[i]);
		if(markerString[i]!=FileMarkerbuffer[i])
		{
			isMarkerCorrect=1;
		}
	}
	
	if(isMarkerCorrect!=0)
	{
		return 1;
	}
	else
	{
		//printf("\n");
		fread(FileVersionbuffer,sizeof(FileVersionbuffer),1,ptr);//should be all 0 so ignore for now
		fread(FileSizebuffer,sizeof(FileSizebuffer),1,ptr);//size of file
	
		//start loop to get all matrix
		for(i=0; i<NY;i++)
		{
			for(j=0;j<NX;j++)
			{
				//get each cell one at a time
				fread(Cellbuffer,sizeof(Cellbuffer),1,ptr); //a cell
				//printf("%x ", Cellbuffer[i]); // prints a series of bytes
				
				if(Cellbuffer[0]>0x00) //is wall
				{
					dungeonMap[j][i]= '#';
					//printf("#");
				}
				else
				{
					dungeonMap[j][i]= '.';
					//printf(".");
				}
			
			}
			//printf("\n");
		}
		fread(NumberOfRoomsbuffer,sizeof(NumberOfRoomsbuffer),1,ptr);//number of rooms
		
		//bellow puts the information for each room(as char array) into array arrayRoomInfo
		int numOfRooms = (int)strtol(NumberOfRoomsbuffer, NULL, 16);
		
		char *arrayRoomInfo[numOfRooms];
		
		for(i=0;i<numOfRooms;i++)
		{
			fread(PosOfRoomsbuffer,sizeof(PosOfRoomsbuffer),1,ptr);
			arrayRoomInfo[i]=PosOfRoomsbuffer;
		}
		return 0;
	}
	
	//first 5 bytes, scmantic file-type marker w/ value RGL229
	//6-9 unsigned 32-bit integer file version w/ value 0
	//10-13 size of this file minus the first 14 lines
	//14-61453 dungeon matrix from top to botom, 4 bytes each cell
	//	first byte(if cell !=0) its open
	//	2nd byte (if != 0) part of room
	//	3rd byte (if != 0) part of coorridor
	//	4th is unsigned 8-bit int for hardness, open space is 0 while unbreakable is 255
	//61454-61455 unsigned 16-bit int with number of rooms
	//61456-end position of all rooms in dungion( 4 unsigned 8-bit ints each)
	//	1st byte is x upper left
	//	2nd byte is y upper left
	//	3rd width of room
	//	4th height of room
	
	int fclose(FILE *fp);
}

	char get_terrain(int x, int y)
	{ 
		return terrain[x][y]; 
	}
	void set_terrain(int x, int y, char setting)
	{
		terrain[x][y] = setting;
	}
	char get_cell_data(int x, int y)
	{
		return cell_data[x][y];
	}

	//render dungion
	//#define NY              160
	//#define NX              96
	//#define numb_of_ROOMS   15

	void print_terrain_all()
	{
		int i, j;
		for (i = 0; i < NX; i++)
		{
			for (j = 0; j < NY; j++)
			{
				//cout << "Hello world" << endl;
				cout << get_terrain(i, j);
			}
			cout << endl;
		}
	}
	void print_cell_data()
	{
		int i, j;
		for (i = 0; i < NX; i++)
		{
			for (j = 0; j < NY; j++)
			{
				//cout << "Hello world" << endl;
				cout << get_cell_data(i,j); // data for each cell, ex: stair, wall, room, edge, coordor
				(i, j);
			}
			cout << endl;
		}
	}
};


int main(int argc, char *argv[])
{
	//int seed = time(NULL);
	srand(time(NULL));
	//char terrain[NX][NY];
	dungeon_map map;// = dungeon_map();
	room_class *room_list;
	room_list = new room_class[numb_of_rooms]; // = room_list;
	//map.terrain;// = floor[NX][NY];
	map.gen_dungeon(*room_list);

	//int i, j;
	//for 
	//	{
	//		map.set_terrain(i, j, '#');
	//	}
	//}
	/*
	for (i = 0; i < NY; i++)
	{
		for (j = 0; j < NX; j++)
		{
			//cout << "Hello world" << endl;
			cout << map.get_terrain(i, j);
		}
		cout << endl;
	}
	
	for (i = 0; i < NY; i++)
	{
		for (j = 0; j < NX; j++)
		{
			//cout << "Hello world" << endl;
			cout << map.get_cell_data(i, j);
		}
		cout << endl;
	}
	*/
	map.print_terrain_all();
	map.print_cell_data();
	//int help = room_list[1].get_height();
	//cout << help << endl;

	//cout << "Hello world" << endl;
	//endl is new line
	system("pause");
	//////////////////////////////////////////////
	/*
	initscr();
	dmode_t m = mode_normal;
	unsigned int seed = time(NULL);
	FILE *f = NULL;
	dungeon.monsters.max = 0;
	if (argc>1)
	{
		if (strcmp(argv[1], "--help") == 0)
		{
			fprintf(stderr, USAGE);
			return 0;
		}
		else if (strcmp(argv[1], "-s") == 0)
		{
			if (argc != 3)
			{
				fprintf(stderr, USAGE);
				return 1;
			}
			int length = strlen(argv[2]), i;
			seed = 0;
			for (i = 0; i<length; i++)
			{
				seed = (seed * 10) + (argv[2][i] - '0');
			}
			printf("Seed: %d\n", seed);
		}
		else if (strcmp(argv[1], "--save") == 0)
		{
			m = mode_save;
		}
		else if (strcmp(argv[1], "--load") == 0)
		{
			m = mode_load;
		}
		else if (strcmp(argv[1], "--nummon") == 0)
		{
			if (argc != 3)
			{
				fprintf(stderr, USAGE);
				return 1;
			}
			int length = strlen(argv[2]), i;
			for (i = 0; i<length; i++)
			{
				dungeon.monsters.max = (10 * dungeon.monsters.max) + (argv[2][i] - '0');
			}
			printf("Selected NumMon as an agrument");
		}
	}
	if (dungeon.monsters.max == 0)
	{
		dungeon.monsters.max = 30;
	}
	dungeon.monsters.max++;//I need 1 spot for the player.
	int size = dungeon.monsters.max * sizeof(monster_t);
	dungeon.monsters.list = malloc(size);
	srand(seed);
	dungeon_init();
	if (m != mode_load)
	{
		generateDungeon();
	}
	else
	{
		if (!open_file(&f, "r"))
		{
			load_dungeon(f);
		}
		else
		{
			fprintf(stderr, "Error loading the file\n");
			return 1;
		}
	}
	//Here is the main game loop
	find_paths();
	char result;
	int i;
	pmode_t mode = pmode_control;
	int scrX = 0, scrY = 0;
	scrX = dungeon.monsters.list[0].x - (SCREEN_W / 2);
	scrY = dungeon.monsters.list[0].y - (SCREEN_H / 2);
	if (scrX < 0) scrX = 0;
	if (scrY < 0) scrY = 0;
	if (scrX >(DUNGEON_X - SCREEN_W)) scrX = DUNGEON_X - SCREEN_W;
	if (scrY >(DUNGEON_Y - SCREEN_H)) scrY = DUNGEON_Y - SCREEN_H;
	printMap(scrX, scrY);
	char in = 0;
	do
	{
		int numDead = 0;
		switch (mode)
		{
		case pmode_control:
			for (i = 0; i<dungeon.monsters.count; i++)
			{
				if (dungeon.monsters.list[i].initiative<0)
				{
					if (!i) result = 2;
					numDead++;
					continue;
				}
				dungeon.monsters.list[i].initiative--;//decrease the initiative
				if (!dungeon.monsters.list[i].initiative)//if initiative counter has reached 0
				{
					dungeon.monsters.list[i].initiative = dungeon.monsters.list[i].speed;//reset the initiative
					if (i == 0)//if the current monster is the player
					{//move the player and the screen
						find_paths();//recalculate the distances to the player
						in = getch();
						switch (in)
						{
						case '7':
						case 'y'://up and left
							if (move_player(-1, -1)) break;
							scrX--;
							scrY--;
							break;
						case '8':
						case 'k'://up
							if (move_player(0, -1)) break;
							scrY--;
							break;
						case '9':
						case 'u'://up and right
							if (move_player(1, -1))break;
							scrY--;
							scrX++;
							break;
						case '6':
						case 'l'://right
							if (move_player(1, 0))break;
							scrX++;
							break;
						case '3':
						case 'n'://down and right
							if (move_player(1, 1))break;
							scrX++;
							scrY++;
							break;
						case '2':
						case 'j'://down
							if (move_player(0, 1))break;
							scrY++;
							break;
						case '1':
						case 'b'://down and left
							if (move_player(-1, 1))break;
							scrX--;
							scrY++;
							break;
						case '4':
						case 'h'://left
							if (move_player(-1, 0))break;
							scrX--;
							break;
						case '<'://down stairs
							//TODO implement stairs
							break;
						case '>'://up stairs
							//TODO implement stairs
							break;
						case 'L'://Look mode
							mode = pmode_look;
							scrX = dungeon.monsters.list[0].x - (SCREEN_W / 2);
							scrY = dungeon.monsters.list[0].y - (SCREEN_H / 2);
							if (scrX < 0) scrX = 0;
							if (scrY < 0) scrY = 0;
							if (scrX >(DUNGEON_X - SCREEN_W)) scrX = DUNGEON_X - SCREEN_W;
							if (scrY >(DUNGEON_Y - SCREEN_H)) scrY = DUNGEON_Y - SCREEN_H;
							break;
						case 'S'://quit
							mode = pmode_quit;
						case ' '://rest for 1 turn.
							break;
						case 27://exit look mode
							//27 is the 'escape' key
							mvprintw(2, 2, "Escape was pressed!");
						default:
							mvprintw(1, 1, "Unbound key: %#o", in);
							break;
						}
						scrX = dungeon.monsters.list[0].x - (SCREEN_W / 2);
						scrY = dungeon.monsters.list[0].y - (SCREEN_H / 2);
						if (scrX < 0) scrX = 0;
						if (scrY < 0) scrY = 0;
						if (scrX >(DUNGEON_X - SCREEN_W)) scrX = DUNGEON_X - SCREEN_W;
						if (scrY >(DUNGEON_Y - SCREEN_H)) scrY = DUNGEON_Y - SCREEN_H;
						printMap(scrX, scrY);//refresh the screen
					}
					else move_monster(i);//otherwise move the monster
				}
				if (dungeon.monsters.list[0].initiative<0)result = 2;
			}
			break;
		case pmode_look:
			in = getch();
			switch (in)
			{
			case '7':
			case 'y'://up and left
				scrX--;
				scrY--;
				break;
			case '8':
			case 'k'://up
				scrY--;
				break;
			case '9':
			case 'u'://up and right
				scrY--;
				scrX++;
				break;
			case '6':
			case 'l'://right
				scrX++;
				break;
			case '3':
			case 'n'://down and right
				scrX++;
				scrY++;
				break;
			case '2':
			case 'j'://down
				scrY++;
				break;
			case '1':
			case 'b'://down and left
				scrX--;
				scrY++;
				break;
			case '4':
			case 'h'://left
				scrX--;
				break;
			case 'S'://quit
				endwin();
				exit(1);
			case ' '://rest for 1 turn.
				break;
			case 27://exit look mode
				mode = pmode_control;
			default:
				mvprintw(1, 1, "Unbound key: %#o", in);
			}
			if (scrX < 0) scrX = 0;
			if (scrY < 0) scrY = 0;
			if (scrX >(DUNGEON_X - SCREEN_W)) scrX = DUNGEON_X - SCREEN_W;
			if (scrY >(DUNGEON_Y - SCREEN_H)) scrY = DUNGEON_Y - SCREEN_H;
			printMap(scrX, scrY);
			break;
		default:
			endwin();
			exit(1);
		}
		if (numDead == dungeon.monsters.count - 1)
		{
			result = 1;
		}
	} while (result == 0 || mode == 1);
	if (result == 2)printf("you Died\n");
	else if (result == 1)printf("Yay, You killed everything, hopfully there was a good reson for that.");
	if (m == mode_save)
	{
		//TODO Save the dungeon to ~/.rlg229/dungeon
		if (!open_file(&f, "w"))
		{
			//TODO save the dungeon to this file
			save_dungeon(f);
		}
		else
		{
			fprintf(stderr, "Error loading the file\n");
			return 1;
		}
	}
	int x;
	for (x = 0; x<DUNGEON_X; x++)
	{
		free(dungeon.map[x]);
	}
	free(dungeon.map);
	free(dungeon.monsters.list);
	if (f)
	{
		fclose(f);
	}
	endwin();

	*/

	return 0;
};

//load 1
//save

//new dungion_map
//generate map(dungion_map)
// in it
	//

//print_map
void print_terrain_all(dungeon_map map)
{
	int i, j;
	for (i = 0; i < NX; i++)
	{
		for (j = 0; j < NY; j++)
		{
			//cout << "Hello world" << endl;
			cout << map.get_terrain(i, j);
		}
		cout << endl;
	}
}
void print_cell_data_all(dungeon_map map)
{
	int i, j;
	for (i = 0; i < NX; i++)
	{
		for (j = 0; j < NY; j++)
		{
			//cout << "Hello world" << endl;
			cout << map.get_cell_data(i, j);
		}
		cout << endl;
	}
}

//swap x&y
/*
void print_terrain_part_centered_pc(int pc_location_x, int pc_location_y, int UL_x, int UL_y, int LR_x, int LR_y, dungeon_map map, PC_class pc) //offset based on char position
{
	//80 x24 around pc //means pc-40 -> pc+40, pc-12 ->pc+12
	////terrain[NY][NX]
	int pc_x = pc.get_location_x();
	int pc_y = pc.get_location_y();
	int i_start = 0; //y
	int j_start = 0; //x
	int i_end = NY;
	int j_end = NX;

	if ((pc_y + 40) >= NY)
	{
		i_start = (NY - 40);
	}
	if ((pc_y - 40) <= 0)
	{
		i_end = 40;
	}
	if ((pc_x + 12) >= NX)
	{
		j_start = (NX - 12);
	}
	if ((pc_x - 12) <= 0)
	{
		j_end = 12;
	}

	int i, j;
	for (i = i_start; i < i_end; i++)
	{
		for (j = j_start; j < j_end; j++)
		{
			//cout << "Hello world" << endl;
			cout << map.get_terrain(i, j);
		}
		cout << endl;
	}
}
*/
