#include <SFML/Graphics.hpp>
#include <time.h>
#include <iostream>
#include <string>
#include <sstream>

#include "Tetris.hpp"

/**********  constructor  **********/
Tetris::Tetris(void)
{
	int temp_figures[7][4] =
	{
			0,2,4,6, // |					0  1
			0,2,3,5, // 4					2  3
			1,3,2,4, // reverse 4			4  5
			1,3,2,5, // -|					6  7
			0,1,3,5, // `|
			1,3,5,4, // _|
			2,3,4,5, // O
	};
	for (int i = 0; i < 7; i++)
		for (int j = 0; j < 4; j++)
			m_figures[i][j] = temp_figures[i][j];


	m_color = 1 + rand() % 6; // rand color
	m_fig = rand() % 7; // rand fig
	for (int i = 0; i < 4; i++)
	{
		m_Point_position[i].x = m_figures[m_fig][i] % 2; // kolumna 0 czy 1
		m_Point_position[i].y = m_figures[m_fig][i] / 2;
	}

	m_next_color = 1 + rand() % 6; // rand color
	m_fig = rand() % 7; // rand fig
	for (int i = 0; i < 4; i++)
	{
		m_Point_next_figure[i].x = m_figures[m_fig][i] % 2; // kolumna 0 czy 1
		m_Point_next_figure[i].y = m_figures[m_fig][i] / 2;
	}

	for (int i = 0; i < 4; i++)
	{
		m_Point_pos_temp[i].x = 0;
		m_Point_pos_temp[i].y = 0;
	}

	m_Centre.x = 0;
	m_Centre.y = 0;

	for (int i = 0; i < X; i++)
		for (int j = 0; j < Y; j++)
			m_game_area[i][j] = 0;

	
}

/**********  getters and setters  **********/
int Tetris::get_color(void) const { return m_color; }
void Tetris::set_color(int new_color) { m_color = new_color; }

int Tetris::get_next_color(void) const { return m_next_color; }
void Tetris::set_next_color(int new_color) { m_next_color = new_color; }

int Tetris::get_fig(void) const { return m_fig; }
void Tetris::set_fig(int new_fig) { m_fig = new_fig; }

Point* Tetris::get_Point_position(void)
{
	Point* Point_position_temp = 0;
	Point_position_temp = new Point[4];

	for (int i = 0; i < 4; i++)
		Point_position_temp[i] = m_Point_position[i];

	return Point_position_temp;
}

Point* Tetris::get_Point_next_figure(void)
{
	Point* Point_next_figure_temp = 0;
	Point_next_figure_temp = new Point[4];

	for (int i = 0; i < 4; i++)
		Point_next_figure_temp[i] = m_Point_next_figure[i];

	return Point_next_figure_temp;
}

int** Tetris::get_game_area(void)
{
	int** game_area_temp = 0;
	game_area_temp = new int* [X];

	for (int i = 0; i < X; i++)
	{
		game_area_temp[i] = new int[Y];

		for (int j = 0; j < Y; j++)
		{
			game_area_temp[i][j] = m_game_area[i][j];
		}
	}
	return game_area_temp;
}


/**********  <- MOVING -> control a figure left or right with the arrow keys  **********/
void Tetris::move_position(int x_position)
{
	for (int i = 0; i < 4; i++)
	{
		m_Point_pos_temp[i] = m_Point_position[i]; // copy
		m_Point_position[i].x += x_position;
	}
	if (move_check())
		for (int j = 0; j < 4; j++) m_Point_position[j] = m_Point_pos_temp[j];
}


/**********  ^ ROTATION ^ rotate the figure with the top arrow key **********/
void Tetris::rotate_figure(void)
{
	for (int j = 0; j < 4; j++) m_Point_pos_temp[j] = m_Point_position[j]; // copy

	m_Centre = m_Point_position[1]; //Centre of rotation

	for (int i = 0; i < 4; i++)
	{
		int x = m_Point_pos_temp[i].y - m_Centre.y;
		int y = m_Point_pos_temp[i].x - m_Centre.x;
		m_Point_pos_temp[i].x = m_Centre.x - x;
		m_Point_pos_temp[i].y = m_Centre.y + y;
	}

	if (rotation_check())
		for (int j = 0; j < 4; j++) m_Point_position[j] = m_Point_pos_temp[j];
}


/**********  v FASTER FALLING v faster figure fall with the low arrow key **********/
void Tetris::fast_falling(void)
{
	for (int i = 0; i < 4; i++)
		m_Point_position[i].y += 1;
}


/**********  check if it is possible to move the figure  **********/
bool Tetris::move_check()
{
	for (int i = 0; i < 4; i++)
	{
		if (m_Point_position[i].x < 0 || m_Point_position[i].x > 10) //gdy punkt wyjdzie poza obszar gry
			return 1;
		else if (m_game_area[m_Point_position[i].y][m_Point_position[i].x]) // kiedy zetknie się z inna figura
			return 1;
	}
	return 0;
}


/**********  check if it is possible to rotate the figure  **********/
bool Tetris::rotation_check()
{
	for (int i = 0; i < 4; i++)
	{
		if (m_Point_pos_temp[i].x < 0 || m_Point_pos_temp[i].x > 10) //gdy punkt wyjdzie poza obszar gry
			return 0;
		else if (m_game_area[m_Point_pos_temp[i].y][m_Point_pos_temp[i].x]) // kiedy zetknie się z inna figura
			return 0;
	}
	return 1;
}


/**********  check if the figure can move down or must stop  **********/
bool Tetris::point_in_free_area()
{
	for (int i = 0; i < 4; i++)
	{
		if (m_game_area[m_Point_position[i].y][m_Point_position[i].x]) // kiedy zetknie się z inna figura
			return 1;
		else if (m_Point_position[i].y > (X - 1)) // kiedy figura zejdzie na sam dół
			return 1;
	}
	return 0;
}


/**********  check if the game is over  **********/
bool Tetris::end_game_check()
{
	for (int i = 0; i < 4; i++)
	{
		if (m_Point_position[i].y == 0 && (m_game_area[m_Point_position[i].y][m_Point_position[i].x] > 0)) // kiedy bedzie figura bedzie u samej gory i zetknie sie z inna figura
			return 1;
	}
	return 0;
}


/**********  clear value of game area  **********/
void Tetris::clear_game_area(void)
{
	for (int i = 0; i < X; i++)
		for (int j = 0; j < Y; j++)
			m_game_area[i][j] = 0;
}


/**********  the function returns the row that is complete. if the row is not complete it returns 0  **********/
int Tetris::which_row_is_full()
{
	for (int i = 0; i < X; i++)
	{
		int temp = 0;

		for (int j = 0; j < Y; j++)
		{
			if (m_game_area[i][j] == 0) // jezeli nie ma klocka to wyjdz z wiersza i nie zliczaj
				break;
			temp++;
		}

		if (temp == Y) // jesli 11 klockow w jednej linii bylo to usun wartosci z tego wiersza
			return i;
	}
	return 0;
}


/**********  deletes the specified row and moves the playing field one position  **********/
void Tetris::clear_row(int row)
{
	for (int k = 0; k < Y; k++)
		m_game_area[row][k] = 0; // remove the values ​​from the row

	for (int k = row; k > 4; k--) // the whole array of values, counting upwards from the row we deleted, we move one position
	{
		for (int j = 0; j < Y; j++)
			m_game_area[k][j] = m_game_area[k - 1][j];
	}
}


/**********  creates consecutive random shapes for the next iterations  **********/
void Tetris::create_figures(void)
{
	for (int i = 0; i < 4; i++)
		m_game_area[m_Point_pos_temp[i].y][m_Point_pos_temp[i].x] = m_color;

	m_color = m_next_color; // losowa liczba 1-7 bo tylko mamy dostepnych kolorow
	m_next_color = 1 + rand() % 6; // losowy kolor dla nastepnej wyswietlanej figury

	m_fig = rand() % 7; // losowa liczba 0-6 bo tyle mamy wierszy w macierzy figur
	for (int i = 0; i < 4; i++)
	{
		m_Point_position[i].x = m_Point_next_figure[i].x; // kolumna 0 czy 1
		m_Point_position[i].y = m_Point_next_figure[i].y;
	}

	m_fig = rand() % 7; // losowa liczba 0-6 bo tyle mamy wierszy w macierzy figur
	for (int i = 0; i < 4; i++)
	{
		m_Point_next_figure[i].x = m_figures[m_fig][i] % 2; // kolumna 0 czy 1
		m_Point_next_figure[i].y = m_figures[m_fig][i] / 2;
	}
}
