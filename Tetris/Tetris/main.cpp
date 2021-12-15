#include <SFML/Graphics.hpp>
#include <time.h>
#include <iostream>

int score = 0;

const char X = 20;
const char Y = 11;

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

struct Point
{
	int x, y;
} Point_position[4], Point_pos_temp[4], Point_next_figure[4];

int game_field[X][Y] = { 0 };


bool point_in_field()
{
	for (int i = 0; i < 4; i++)
	{
		if (game_field[Point_position[i].y][Point_position[i].x] > 0) // kiedy zetknie siê z inna figura
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
		if (Point_position[i].y == 0 && (game_field[Point_position[i].y][Point_position[i].x] > 0)) // kiedy bedzie figura bedzie u samej gory i zetknie sie z inna figura
			return 1;
	}
	return 0;
}


bool move_check()
{
	for (int i = 0; i < 4; i++)
	{
		if (Point_position[i].x < 0 || Point_position[i].x > 10) // czy punkt znajduje sie w obszarze gry
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
		//else if (Point_pos_temp[i].x) // gdy figura jest kwadratem
			//return 0;
	}
	return 1;
}


int main()
{
	sf::RenderWindow window(sf::VideoMode(480, 396), "Tetris game");

	sf::Texture tex1, tex2, tex3;
	tex1.loadFromFile("images/colors.png");
	tex2.loadFromFile("images/tetrisbackground.png");

	sf::Sprite spr1, spr2, spr3;
	spr1.setTexture(tex1);
	spr2.setTexture(tex2);
	spr3.setTexture(tex3);

	bool rotate = 0; // do obracania figura
	int x_position = 0; // do sterowania strzalkami 
	int color = 0; //wylosowany kolor dla figury
	int next_color = 0; //wylosowany kolor dla nastepnej figury
	int row; // wylosowany wiersz figury
	int end_game = 0; // konczy gre
	
	srand(time(NULL));

	sf::Clock clock;
	float timer = 0, delay = 0.3;

	spr1.setTextureRect(sf::IntRect(color * 18, 0, 18, 18)); // wycina jeden kwadrat z calej tekstury (losowy kolor)

	sf::Font font; // obiekt do czcionki
	if (!font.loadFromFile("fonts/FT_BetonBold.otf"))
		std::cout << "cannot load fonts" << std::endl;
	sf::Text text; //obiekt do tekstu 
	text.setFont(font); // ustawiamy czcionke ktora wczytalismy

	while (window.isOpen())
	{
		float time = clock.getElapsedTime().asSeconds(); // zwraca czas w sekundach od ostatniego momentu uzycia clock.restart()
		clock.restart();
		timer += time;

		sf::Event eve;
		while (window.pollEvent(eve))
		{
			if (eve.type == sf::Event::Closed)
				window.close();
			if (eve.type == sf::Event::KeyPressed)
			{
				if (eve.key.code == sf::Keyboard::Up) rotate = true;
				else if (eve.key.code == sf::Keyboard::Right) x_position = 1;
				else if (eve.key.code == sf::Keyboard::Left) x_position = -1;
			}

		}

		/**********  <- MOVING ->  **********/
		for (int i = 0; i < 4; i++)
		{
			Point_pos_temp[i] = Point_position[i]; // copy
			Point_position[i].x += x_position;
		}
		if (move_check())
			for (int j = 0; j < 4; j++) Point_position[j] = Point_pos_temp[j];


		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) delay = 0.05;

		/**********  ^ ROTATION ^  **********/

		if (rotate)
		{
			for (int j = 0; j < 4; j++) Point_pos_temp[j] = Point_position[j]; // copy

			Point centrum = Point_position[1];
		
			for (int i = 0; i < 4; i++)
			{
				int x = Point_pos_temp[i].y - centrum.y;
				int y = Point_pos_temp[i].x - centrum.x;
				Point_pos_temp[i].x = centrum.x - x;
				Point_pos_temp[i].y = centrum.y + y;
			}
	
			if (rotation_check())
				for (int j = 0; j < 4; j++) Point_position[j] = Point_pos_temp[j];
			

		}

		/**********  v FASTER FALLING v  **********/
		if (timer > delay)
		{
			for (int i = 0; i < 4; i++)
			{
				Point_pos_temp[i] = Point_position[i];
				Point_position[i].y += 1;
			}
			timer = 0; // zerujemy timer
		}


		/**********  __ COMPLETE LINE DELETE __  **********/
		for (int i = 0; i < X; i++)
		{
			int temp = 0;
			int row_temp = 0;

			for (int j = 0; j < Y; j++)
			{
				if (game_field[i][j] == 0) // jezeli nie ma klocka to wyjdz z wiersza i nie zliczaj
					break;
				temp++;
			}
			
			if (temp == Y) // jesli 11 klockow w jednej linii bylo to usun wartosci z tego wiersza
			{
				score++; // aktualnt wynik
				row_temp = i; // zapisujemy wartosc wiersza w ktorym jest kompletna linia
				for (int k = 0; k < Y; k++)
					game_field[row_temp][k] = 0; // usuwamy z niej wartosci

				for (int k = row_temp; k > 4; k--) // cala tablice wartosci liczac w gore od wiersza ktorego usunelismy przesuwamy o jedna pozycje 
				{
					for (int j = 0; j < Y; j++)
						game_field[k][j] = game_field[k - 1][j];
				}
			}		
		}
		

		/**********  __ END OF GAME MOMENT __  **********/
		if (end_game_check())
			end_game = 1;

		if (end_game == 1)
		{
			while (1)
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
				{
					for (int i = 0; i < X; i++)
						for (int j = 0; j < Y; j++)
							game_field[i][j] = 0;		// czyscimy zawartosc obszaru gry
					end_game = 0;
					break;
				}
			}
		}



		////
		if (point_in_field())
		{
			for (int i = 0; i < 4; i++)
			{
				game_field[Point_pos_temp[i].y][Point_pos_temp[i].x] = color;
			}

			color = 1 + rand() % 7; // losowa liczba 1-7 bo tylko mamy dostepnych kolorow
			row = rand() % 7; // losowa liczba 0-6 bo tyle mamy wierszy w macierzy figur
			for (int i = 0; i < 4; i++)
			{
				Point_position[i].x = figures[row][i] % 2; // kolumna 0 czy 1
				Point_position[i].y = figures[row][i] / 2;
			}

			next_color = 1 + rand() % 7; // losowy kolor dla nastepnej wyswietlanej figury
			row = rand() % 7; // losowa liczba 0-6 bo tyle mamy wierszy w macierzy figur
			for (int i = 0; i < 4; i++)
			{
				Point_next_figure[i].x = figures[row][i] % 2; // kolumna 0 czy 1
				Point_next_figure[i].y = figures[row][i] / 2;
			}
		}


		window.clear(sf::Color::Black);
		window.draw(spr2); // background

		/**********  __ DRAW ACTUALLY FIGURE __  **********/
		for (int i = 0; i < 4; i++)
		{
			spr1.setTextureRect(sf::IntRect(color * 18, 0, 18, 18));
			spr1.setPosition(Point_position[i].x * 18, Point_position[i].y * 18);
			spr1.move(18, 18); // przesuwanie obiektu wzglêdem jego aktualnej pozycji x // zeby dopasowac do tla
			window.draw(spr1);
		}


		/**********  __ DRAW ALL FIGURE __  **********/
		for (int i = 0; i < X; i++)
		{
			for (int j = 0; j < Y; j++)
			{
				if (game_field[i][j] == 0) continue; // kiedy niewypelnilismy pola zadna figura to pomin
				spr1.setTextureRect(sf::IntRect(game_field[i][j] * 18, 0, 18, 18)); // wycina kawalek z calej tekstury 
				spr1.setPosition(j * 18, i * 18); // sprajt zostanie umieszczony na pozycji x,y
				spr1.move(18, 18); // przesuwanie obiektu wzglêdem jego aktualnej pozycji x // zeby dopasowac do tla
				window.draw(spr1);
			}
		}

		/**********  __ DRAW NEXT FIGURE __  **********/
		
			for (int i = 0; i < 4; i++)
			{
				spr1.setTextureRect(sf::IntRect(next_color * 18, 0, 18, 18));
				spr1.setPosition(Point_next_figure[i].x * 18, Point_next_figure[i].y * 18);
				spr1.move(328, 58); // przesuwanie obiektu wzglêdem jego aktualnej pozycji x // zeby dopasowac do tla
				window.draw(spr1);
			}
		
		/**********  __ DRAW SCORE __  **********/
		text.setString("YOUR SCORE:");
		text.setCharacterSize(30); 
		text.setFillColor(sf::Color::White);
		window.draw(text);



		/**********  __ DISPLAY ALL AND RESET VARIABLES __  **********/
		window.display();
		x_position = 0; // musimy wyzerowac wartosc kiedy juz ustawilismy pozycje sprite 
		delay = 0.3; // powrot do standardowego czasu opadania
		rotate = 0; // skasuj rotacje

	}
	return 0;
}