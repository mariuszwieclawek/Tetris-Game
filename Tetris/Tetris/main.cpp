#include <SFML/Graphics.hpp>
#include <time.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

const char X = 20;
const char Y = 11;

struct Point
{
	int x, y;
};

class Tetris
{
private:
	int m_color = 0; //wylosowany kolor dla figury
	int m_next_color = 0; //wylosowany kolor dla nastepnej figury
	int m_fig; // wylosowana figura

	int game_area[X][Y] = { 0 };

	struct Point Point_position[4], Point_pos_temp[4], Point_next_figure[4], Centre;

	int figures[7][4] =			//mozliwy wyglad figur
	{
		0,2,4,6, // |					0  1
		0,2,3,5, // 4					2  3
		1,3,2,4, // reverse 4			4  5
		1,3,2,5, // -|					6  7
		0,1,3,5, // `|
		1,3,5,4, // _|
		2,3,4,5, // O
	};

public:
	int get_color(void) const { return m_color; }
	void set_color(int new_color) { m_color = new_color; }

	int get_next_color(void) const { return m_next_color; }
	void set_next_color(int new_color) { m_next_color = new_color; }

	int get_fig(void) const { return m_fig; }
	void set_fig(int new_fig) { m_fig = new_fig; }
	
	Point* get_Point_position(void)
	{
		Point* Point_position_temp = 0;
		Point_position_temp = new Point[4];

		for (int i = 0; i < 4; i++)
			Point_position_temp[i] = Point_position[i];

		return Point_position_temp;
	}

	Point* get_Point_next_figure(void)
	{
		Point* Point_next_figure_temp = 0;
		Point_next_figure_temp = new Point[4];

		for (int i = 0; i < 4; i++)
			Point_next_figure_temp[i] = Point_next_figure[i];

		return Point_next_figure_temp;
	}

	int** get_game_area(void)
	{
		int** game_area_temp = 0;
		game_area_temp = new int* [X];

		for (int i = 0; i < X; i++)
		{
			game_area_temp[i] = new int[Y];

			for (int j = 0; j < Y; j++)
			{
				game_area_temp[i][j] = game_area[i][j];
			}
		}

		return game_area_temp;
	}


	/**********  check if the figure can move down or must stop  **********/
	bool point_in_free_area()
	{
		for (int i = 0; i < 4; i++)
		{
			if (game_area[Point_position[i].y][Point_position[i].x]) // kiedy zetknie się z inna figura
				return 1;
			else if (Point_position[i].y > (X - 1)) // kiedy figura zejdzie na sam dół
				return 1;
		}

		return 0;
	}

	/**********  check if the game is over  **********/
	bool end_game_check()
	{
		for (int i = 0; i < 4; i++)
		{
			if (Point_position[i].y == 0 && (game_area[Point_position[i].y][Point_position[i].x] > 0)) // kiedy bedzie figura bedzie u samej gory i zetknie sie z inna figura
				return 1;
		}
		return 0;
	}

	/**********  check if it is possible to move the figure  **********/
	bool move_check()
	{
		for (int i = 0; i < 4; i++)
		{
			if (Point_position[i].x < 0 || Point_position[i].x > 10) //gdy punkt wyjdzie poza obszar gry
				return 1;
			else if (game_area[Point_position[i].y][Point_position[i].x]) // kiedy zetknie się z inna figura
				return 1;
		}
		return 0;
	}

	/**********  check if it is possible to rotate the figure  **********/
	bool rotation_check()
	{
		for (int i = 0; i < 4; i++)
		{
			if (Point_pos_temp[i].x < 0 || Point_pos_temp[i].x > 10) //gdy punkt wyjdzie poza obszar gry
				return 0;
			else if (game_area[Point_pos_temp[i].y][Point_pos_temp[i].x]) // kiedy zetknie się z inna figura
				return 0;
		}
		return 1;
	}

	/**********  <- MOVING ->  **********/
	void move_position(int x_position)
	{
		for (int i = 0; i < 4; i++)
		{
			Point_pos_temp[i] = Point_position[i]; // copy
			Point_position[i].x += x_position;
		}
		if (move_check())
			for (int j = 0; j < 4; j++) Point_position[j] = Point_pos_temp[j];
	}

	/**********  ^ ROTATION ^  **********/
	void rotate_figure(void)
	{
		for (int j = 0; j < 4; j++) Point_pos_temp[j] = Point_position[j]; // copy

		Centre = Point_position[1]; //Centre of rotation

		for (int i = 0; i < 4; i++)
		{
			int x = Point_pos_temp[i].y - Centre.y;
			int y = Point_pos_temp[i].x - Centre.x;
			Point_pos_temp[i].x = Centre.x - x;
			Point_pos_temp[i].y = Centre.y + y;
		}

		if (rotation_check())
			for (int j = 0; j < 4; j++) Point_position[j] = Point_pos_temp[j];
	}

	/**********  v FASTER FALLING v  **********/
	void fast_falling(void)
	{
		for (int i = 0; i < 4; i++)
		{
			//Point_pos_temp[i] = Point_position[i];
			Point_position[i].y += 1;
		}
	}

	/**********  CLEAR VALUE OF GAME AREA  **********/
	void clear_game_area(void)
	{
		for (int i = 0; i < X; i++)
			for (int j = 0; j < Y; j++)
				game_area[i][j] = 0;
	}

	/**********  the function returns the row that is complete. if the row is not complete it returns 0  **********/
	int which_row_is_full()
	{
		for (int i = 0; i < X; i++)
		{
			int temp = 0;

			for (int j = 0; j < Y; j++)
			{
				if (game_area[i][j] == 0) // jezeli nie ma klocka to wyjdz z wiersza i nie zliczaj
					break;
				temp++;
			}

			if (temp == Y) // jesli 11 klockow w jednej linii bylo to usun wartosci z tego wiersza
				return i;
		}
		return 0;
	}

	/**********  deletes the specified row and moves the playing field one position  **********/
	void clear_row(int row)
	{
		for (int k = 0; k < Y; k++)
			game_area[row][k] = 0; // remove the values ​​from the row

		for (int k = row; k > 4; k--) // the whole array of values, counting upwards from the row we deleted, we move one position
		{
			for (int j = 0; j < Y; j++)
				game_area[k][j] = game_area[k - 1][j];
		}
	}

	void create_first_figures(void)
	{
		m_color = 1 + rand() % 7; // rand color
		m_fig = rand() % 7; // rand fig
		for (int i = 0; i < 4; i++)
		{
			Point_position[i].x = figures[m_fig][i] % 2; // kolumna 0 czy 1
			Point_position[i].y = figures[m_fig][i] / 2;
		}

		m_next_color = 1 + rand() % 7; // rand color
		m_fig = rand() % 7; // rand fig
		for (int i = 0; i < 4; i++)
		{
			Point_next_figure[i].x = figures[m_fig][i] % 2; // kolumna 0 czy 1
			Point_next_figure[i].y = figures[m_fig][i] / 2;
		}
	}
	

	void create_next_figures(void)
	{
		for (int i = 0; i < 4; i++)
			game_area[Point_pos_temp[i].y][Point_pos_temp[i].x] = m_color;
		

		m_color = m_next_color; // losowa liczba 1-7 bo tylko mamy dostepnych kolorow
		m_next_color = 1 + rand() % 7; // losowy kolor dla nastepnej wyswietlanej figury

		m_fig = rand() % 7; // losowa liczba 0-6 bo tyle mamy wierszy w macierzy figur
		for (int i = 0; i < 4; i++)
		{
			Point_position[i].x = Point_next_figure[i].x; // kolumna 0 czy 1
			Point_position[i].y = Point_next_figure[i].y;
		}

		m_fig = rand() % 7; // losowa liczba 0-6 bo tyle mamy wierszy w macierzy figur
		for (int i = 0; i < 4; i++)
		{
			Point_next_figure[i].x = figures[m_fig][i] % 2; // kolumna 0 czy 1
			Point_next_figure[i].y = figures[m_fig][i] / 2;
		}
	}

};


int main()
{	
	Tetris tetris;

	bool rotate = 0; // do obracania figura
	int x_position = 0; // do sterowania strzalkami 
	int end_game = 0; // konczy gre
	int score = 0; // Aktualny wynik
	int best_score = 0; // Najlepszy wynik
	float timer = 0; //do zliczania czasu
	double delay = 0.3; //szybkosc opadania
	int full_row; //ktory wiersz zostal zapelniony w calosci
	int first_iteration = 1; // dla pierwszej iteracji aby zainicjalizowac zmienne 

	sf::RenderWindow window(sf::VideoMode(460, 396), "Tetris game");

	sf::Texture tex1, tex2;
	tex1.loadFromFile("images/colors.png");
	tex2.loadFromFile("images/tetrisbackground.png");

	sf::Sprite spr1, spr2;
	spr1.setTexture(tex1);
	spr2.setTexture(tex2);

	sf::Event eve; 

	sf::Clock clock;

	sf::Font font;
	font.loadFromFile("fonts/FT_BetonBold.otf");
		
	sf::Text text; 
	text.setFont(font); 

	
	/********************************************************************** MAIN LOOP **********************************************************************/
	while (window.isOpen())
	{
		srand((unsigned int)time(NULL));
		float time = clock.getElapsedTime().asSeconds(); // zwraca czas w sekundach od ostatniego momentu uzycia clock.restart()
		clock.restart();
		timer += time;


		/**********  EVENT DETECTION  **********/
		while (window.pollEvent(eve))
		{
			if (eve.type == sf::Event::Closed)
				window.close();
			if (eve.type == sf::Event::KeyPressed)
			{
				if (eve.key.code == sf::Keyboard::Up) rotate = true;
				else if (eve.key.code == sf::Keyboard::Right) x_position = 1;
				else if (eve.key.code == sf::Keyboard::Left) x_position = -1;
				else if (eve.key.code == sf::Keyboard::Escape) window.close();
			}

		}


		/**********  <- MOVING ->  **********/
		tetris.move_position(x_position);
	

		/**********  ^ ROTATION ^  **********/
		if (rotate)
			tetris.rotate_figure();


		/**********  v FASTER FALLING v  **********/
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) delay = 0.05;
		if (timer > delay)
		{
			tetris.fast_falling();
			timer = 0; // zerujemy timer
		}


		/**********  DELETE ROW WHEN IS FULL  **********/
		if (full_row = tetris.which_row_is_full())
		{
			score++; // aktualny wynik	
			tetris.clear_row(full_row); // usuwamy wiersz ktory sie zapelnil i przesuwamy obszar gry o jedna pozycje wzgledem usunietego wiersza
		}


		/**********   CREATE RANDOM FIGURE  **********/
			// Init values when the programs starts
		if (first_iteration)
		{
			tetris.create_first_figures();
			first_iteration = 0;
		}
			// Create next figures and control if the figure is in free game area
		if (tetris.point_in_free_area())
			tetris.create_next_figures();


		/**********   END OF GAME MOMENT   **********/
		if (tetris.end_game_check())
		{
			while (1)
			{
				text.setCharacterSize(30);
				text.setFillColor(sf::Color::Red);
				text.setPosition(280, 300);
				text.setString("GAME OVER !");
				window.draw(text);
				text.setCharacterSize(20);
				text.setPosition(300, 330);
				text.setString("press space");
				window.draw(text);
				window.display();
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) // kiedy klikniemy escape zamykamy okno
					window.close();
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) // kiedy klikniemy spacje nowa gra
				{
					tetris.clear_game_area(); // czyscimy zawartosc obszaru gry
					end_game = 0;
					if (score > best_score) // sprawdzamy czy to najlepszy wynik
						best_score = score;
					score = 0;
					break;
				}
			}
		}


		/********************************************************************** DISPLAYS **********************************************************************/
		window.clear(sf::Color::Black);
		window.draw(spr2); // background

		int **game_area = tetris.get_game_area(); // get values of game area
		Point* Point_position = tetris.get_Point_position();
		Point* Point_next_figure = tetris.get_Point_next_figure();
		
		/**********   DRAW ACTUALLY FIGURE   **********/
		for (int i = 0; i < 4; i++)
		{
			spr1.setTextureRect(sf::IntRect(tetris.get_color() * 18, 0, 18, 18));
			spr1.setPosition(float(Point_position[i].x * 18), float(Point_position[i].y * 18));
			spr1.move(18, 18); // przesuwanie obiektu względem jego aktualnej pozycji x // zeby dopasowac do tla
			window.draw(spr1);
		}	


		/**********   DRAW ALL FIGURE   **********/
		for (int i = 0; i < X; i++)
		{
			for (int j = 0; j < Y; j++)
			{
				if (game_area[i][j] == 0) continue; // kiedy niewypelnilismy pola zadna figura to pomin
				spr1.setTextureRect(sf::IntRect(game_area[i][j] * 18, 0, 18, 18)); // wycina kawalek z calej tekstury 
				spr1.setPosition(float(j * 18),float(i * 18)); // sprajt zostanie umieszczony na pozycji x,y
				spr1.move(18, 18); // przesuwanie obiektu względem jego aktualnej pozycji x // zeby dopasowac do tla
				window.draw(spr1);
			}
		}


		/**********   DRAW NEXT FIGURE   **********/
		for (int i = 0; i < 4; i++)
		{
			spr1.setTextureRect(sf::IntRect(tetris.get_next_color() * 18, 0, 18, 18));
			spr1.setPosition(float(Point_next_figure[i].x * 18), float(Point_next_figure[i].y * 18));
			spr1.move(328, 58); // przesuwanie obiektu względem jego aktualnej pozycji x // zeby dopasowac do tla
			window.draw(spr1);
		}
		

		/**********   DRAW SCORE AND SOME FUNCTIONALLITY  **********/
		std::stringstream ss1,ss2;
		ss1 << score;
		ss2 << best_score;
		std::string str1 = ss1.str();
		std::string str2 = ss2.str();

		text.setCharacterSize(30);
		text.setFillColor(sf::Color::White);
		text.setPosition(280, 180);
		text.setString("YOUR SCORE:");
		window.draw(text);
		text.setPosition(340, 210);
		text.setString(str1);
		window.draw(text);
		text.setPosition(280, 240);
		text.setString("BEST SCORE:");
		window.draw(text);
		text.setPosition(340, 270);
		text.setString(str2);
		window.draw(text);
		text.setCharacterSize(15);
		text.setPosition(295, 360);
		text.setString("press ESC to exit");
		window.draw(text);


		/**********   DISPLAY ALL AND RESET VARIABLES   **********/
		window.display();
		x_position = 0; // musimy wyzerowac wartosc kiedy juz ustawilismy pozycje sprite 
		delay = 0.3; // powrot do standardowego czasu opadania
		rotate = 0; // skasuj rotacje


		/**********   CLEAN MEMORY   **********/
		for (int i = 0; i < X; i++)
		{
			delete[] game_area[i];
		}
		delete[] game_area;
		game_area = 0;

		delete[] Point_position;
		Point_position = 0;
		delete[] Point_next_figure;
		Point_next_figure = 0;

	}
	return 0;
}