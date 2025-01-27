#include <ncurses.h>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <string>

using namespace std;

const int GAME_WIDTH = 20;
const int GAME_HEIGHT = 30;

struct Car {
    int x;
    int y;
    int speed_y;
};

// Определяем оператор == для Car
bool operator==(const Car& a, const Car& b) {
    return a.x == b.x && a.y == b.y && a.speed_y == b.speed_y;
}

int main() {
    // Инициализация curses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    // Создание игровых окон
    WINDOW* game_win = newwin(GAME_HEIGHT, GAME_WIDTH, 0, 0);
    WINDOW* score_win = newwin(3, GAME_WIDTH, GAME_HEIGHT, 0);

    // Инициализация игрока
    Car player;
    player.x = GAME_WIDTH / 2;
    player.y = GAME_HEIGHT - 2;
    player.speed_y = 1;

    // Вектор для хранения других автомобилей
    vector<Car> other_cars;

    // Настройка времени обновления
    timeout(100); // 100 ms (10 ms game time)

    int score = 0;
    bool game_over = false;

    while (!game_over) {
        // Обработка ввода
        int ch = getch();
        if (ch != ERR) {
            switch(ch) {
                case KEY_LEFT: if (player.x > 1) player.x--; break;
                case KEY_RIGHT: if (player.x < GAME_WIDTH - 2) player.x++; break;
            }
        }

        // Проверка столкновения с другими автомобилями
        for (auto& c : other_cars) {
            if (c.x == player.x && c.y == player.y) {
                game_over = true;
                break;
            }
        }

        // Проверка столкновения с границами экрана
        if (player.x <= 0 || player.x >= GAME_WIDTH - 1 || 
            player.y <= 0 || player.y >= GAME_HEIGHT - 1) {
            game_over = true;
            break;
        }

        if (game_over) break;

        // Генерация новых авто чаще (every 25 iterations)
        if (rand() % 25 == 0) { 
            Car new_car;
            new_car.x = 1 + rand() % (GAME_WIDTH - 4); // RANDOMIZE по X
            new_car.y = 1;             // Появляются сверху
            new_car.speed_y = 1 + rand() % 3; // Random speed between 1 and 2
            other_cars.push_back(new_car);
        }

        // Движение других машин вниз
        vector<Car> new_cars;
        for (auto& c : other_cars) {
            c.y += c.speed_y; 

            // Удаляем машины, которые ушли вниз за пределы экрана
            if (c.y < GAME_HEIGHT - 1) {
                new_cars.push_back(c);
            }
        }
        other_cars = new_cars;

        // Обновление экрана
        werase(game_win);
        // Рисование границ
        for (int x = 0; x < GAME_WIDTH; x++) {
            mvwaddch(game_win, 0, x, '#');
            mvwaddch(game_win, GAME_HEIGHT - 1, x, '#');
        }
        for (int y = 0; y < GAME_HEIGHT; y++) {
            mvwaddch(game_win, y, 0, '#');
            mvwaddch(game_win, y, GAME_WIDTH - 1, '#');
        }

        // Рисование других авто
        for (auto& c : other_cars) {
            mvwaddch(game_win, c.y, c.x, '&');
        }

        // Рисование игрока
        mvwaddch(game_win, player.y, player.x, '@');

        werase(score_win);
        score += 10; // увеличиваем очки быстрее
        mvwprintw(score_win, 0, 0, "Score: %d", score);

        wrefresh(game_win);
        wrefresh(score_win);
    }

    // Game Over
    werase(game_win);
    mvwprintw(game_win, GAME_HEIGHT / 2, (GAME_WIDTH - 10)/2, "Game Over!");
    mvwprintw(game_win, GAME_HEIGHT / 2 + 1, (GAME_WIDTH - 8)/2, "Score: %d", score);
    wrefresh(game_win);

    // Очистка и выход
    endwin();

    return 0;
}
