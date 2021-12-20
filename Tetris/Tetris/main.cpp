#include <SFML/Graphics.hpp>
#include <time.h>
#include <iostream>
#include <string>
#include <sstream>

const char X = 20;
const char Y = 11;

class Tetris
{
private:



public:
	struct Point
	{
		int x, y;
	} Point_position[4], Point_pos_temp[4], Point_next_figure[4], Centrum;

	int game_area[X][Y] = { 0 };

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

	int * get_figures(void) { return *figures; }
	int * get_game_area(void) { return *game_area; }
	void set_game_area(int (&tab)[X][Y]) 
	{ 
		for (int i = 0; i < X; i++)
			for (int j = 0; j < Y; j++)
				game_area[i][j] = tab[i][j];
	}


	bool point_in_free_area()
	{
		for (int i = 0; i < 4; i++)
		{
			if (game_area[Point_position[i].y][Point_position[i].x]) // kiedy zetknie siê z inna figura
				return 1;
			else if (Point_position[i].y > (X - 1)) // kiedy figura zejdzie na sam dó³
				return 1;
		}

		return 0;
	}


	bool end_game_check()
	{
		for (int i = 0; i < 4; i++)
		{
			if (Point_position[i].y == 0 && (game_area[Point_position[i].y][Point_position[i].x] > 0)) // kiedy bedzie figura bedzie u samej gory i zetknie sie z inna figura
				return 1;
		}
		return 0;
	}


	bool move_check()
	{
		for (int i = 0; i < 4; i++)
		{
			if (Point_position[i].x < 0 || Point_position[i].x > 10) //gdy punkt wyjdzie poza obszar gry
				return 1;
			else if (game_area[Point_position[i].y][Point_position[i].x]) // kiedy zetknie siê z inna figura
				return 1;
		}
		return 0;
	}


	bool rotation_check()
	{
		for (int i = 0; i < 4; i++)
		{
			if (Point_pos_temp[i].x < 0 || Point_pos_temp[i].x > 10) //gdy punkt wyjdzie poza obszar gry
				return 0;
			else if (game_area[Point_pos_temp[i].y][Point_pos_temp[i].x]) // kiedy zetknie siê z inna figura
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

		Centrum = Point_position[1]; //centrum of rotation

		for (int i = 0; i < 4; i++)
		{
			int x = Point_pos_temp[i].y - Centrum.y;
			int y = Point_pos_temp[i].x - Centrum.x;
			Point_pos_temp[i].x = Centrum.x - x;
			Point_pos_temp[i].y = Centrum.y + y;
		}

		if (rotation_check())
			for (int j = 0; j < 4; j++) Point_position[j] = Point_pos_temp[j];
	}

	/**********  v FASTER FALLING v  **********/
	void fast_falling(void)
	{
		for (int i = 0; i < 4; i++)
		{
			Point_pos_temp[i] = Point_position[i];
			Point_position[i].y += 1;
		}
	}

};


int main()
{	
	Tetris tetris;
	bool rotate = 0; // do obracania figura
	int x_position = 0; // do sterowania strzalkami 
	int color = 0; //wylosowany kolor dla figury
	int next_color = 0; //wylosowany kolor dla nastepnej figury
	int row; // wylosowany wiersz figury
	int end_game = 0; // konczy gre
	int score = 0; // Aktualny wynik
	int best_score = 0; // Najlepszy wynik
	float timer = 0; //do zliczania czasu
	double delay = 0.3; //szybkosc opadania

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
	
	/********************  FIRST RANDOM FIGURE  ********************/
	
	color = 1 + rand() % 7; // rand color
	row = rand() % 7; // rand row
	for (int i = 0; i < 4; i++)
	{
		tetris.Point_position[i].x = tetris.figures[row][i] % 2; // kolumna 0 czy 1
		tetris.Point_position[i].y = tetris.figures[row][i] / 2;
	}
	
	next_color = 1 + rand() % 7; // rand color
	row = rand() % 7; // rand row
	for (int i = 0; i < 4; i++)
	{
		tetris.Point_next_figure[i].x = tetris.figures[row][i] % 2; // kolumna 0 czy 1
		tetris.Point_next_figure[i].y = tetris.figures[row][i] / 2;
	}
	
	/*******************************************************************************/
	/******************				   MAIN LOOP	 			 *******************/
	/*******************************************************************************/
	while (window.isOpen())
	{
		srand(time(NULL));
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


		/**********  __ COMPLETE LINE DELETE __  **********/
		for (int i = 0; i < X; i++)
		{
			int temp = 0;
			int row_temp = 0;

			for (int j = 0; j < Y; j++)
			{
				if (tetris.game_area[i][j] == 0) // jezeli nie ma klocka to wyjdz z wiersza i nie zliczaj
					break;
				temp++;
			}
			
			if (temp == Y) // jesli 11 klockow w jednej linii bylo to usun wartosci z tego wiersza
			{
				score++; // aktualny wynik
				row_temp = i; // zapisujemy wartosc wiersza w ktorym jest kompletna linia
				for (int k = 0; k < Y; k++)
					tetris.game_area[row_temp][k] = 0; // usuwamy z niej wartosci

				for (int k = row_temp; k > 4; k--) // cala tablice wartosci liczac w gore od wiersza ktorego usunelismy przesuwamy o jedna pozycje 
				{
					for (int j = 0; j < Y; j++)
						tetris.game_area[k][j] = tetris.game_area[k - 1][j];
				}
			}		
		}
		

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
					for (int i = 0; i < X; i++)
						for (int j = 0; j < Y; j++)
							tetris.game_area[i][j] = 0;		// czyscimy zawartosc obszaru gry
					end_game = 0;
					if (score > best_score) // sprawdzamy czy to najlepszy wynik
						best_score = score;
					score = 0;
					break;
				}
			}
		}



		/**********   CREATE RANDOM FIGURE  **********/
		if (tetris.point_in_free_area())
		{
			for (int i = 0; i < 4; i++)
			{
				tetris.game_area[tetris.Point_pos_temp[i].y][tetris.Point_pos_temp[i].x] = color;
			}

			color = next_color; // losowa liczba 1-7 bo tylko mamy dostepnych kolorow
			next_color = 1 + rand() % 7; // losowy kolor dla nastepnej wyswietlanej figury

			row = rand() % 7; // losowa liczba 0-6 bo tyle mamy wierszy w macierzy figur
			for (int i = 0; i < 4; i++)
			{
				tetris.Point_position[i].x = tetris.Point_next_figure[i].x; // kolumna 0 czy 1
				tetris.Point_position[i].y = tetris.Point_next_figure[i].y;
			}

			row = rand() % 7; // losowa liczba 0-6 bo tyle mamy wierszy w macierzy figur
			for (int i = 0; i < 4; i++)
			{
				tetris.Point_next_figure[i].x = tetris.figures[row][i] % 2; // kolumna 0 czy 1
				tetris.Point_next_figure[i].y = tetris.figures[row][i] / 2;
			}
		}




		/*******************************************************************************/
		/********************************DISPLAY DRAW***********************************/
		/*******************************************************************************/

		window.clear(sf::Color::Black);
		window.draw(spr2); // background

		/**********   DRAW ACTUALLY FIGURE   **********/
		for (int i = 0; i < 4; i++)
		{
			spr1.setTextureRect(sf::IntRect(color * 18, 0, 18, 18));
			spr1.setPosition(tetris.Point_position[i].x * 18, tetris.Point_position[i].y * 18);
			spr1.move(18, 18); // przesuwanie obiektu wzglêdem jego aktualnej pozycji x // zeby dopasowac do tla
			window.draw(spr1);
		}	

		/**********   DRAW ALL FIGURE   **********/
		for (int i = 0; i < X; i++)
		{
			for (int j = 0; j < Y; j++)
			{
				if (tetris.game_area[i][j] == 0) continue; // kiedy niewypelnilismy pola zadna figura to pomin
				spr1.setTextureRect(sf::IntRect(tetris.game_area[i][j] * 18, 0, 18, 18)); // wycina kawalek z calej tekstury 
				spr1.setPosition(j * 18, i * 18); // sprajt zostanie umieszczony na pozycji x,y
				spr1.move(18, 18); // przesuwanie obiektu wzglêdem jego aktualnej pozycji x // zeby dopasowac do tla
				window.draw(spr1);
			}
		}

		/**********   DRAW NEXT FIGURE   **********/
		
			for (int i = 0; i < 4; i++)
			{
				spr1.setTextureRect(sf::IntRect(next_color * 18, 0, 18, 18));
				spr1.setPosition(tetris.Point_next_figure[i].x * 18, tetris.Point_next_figure[i].y * 18);
				spr1.move(328, 58); // przesuwanie obiektu wzglêdem jego aktualnej pozycji x // zeby dopasowac do tla
				window.draw(spr1);
			}
		
		/**********   DRAW SCORE etc  **********/
	
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

	}
	return 0;
}