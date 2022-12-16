#include <Graph_lib/Simple_window.h>
#include <std_lib_facilities.h>
#include <ctime>

using Graph_lib::Point;
using namespace std;

struct Cell: Graph_lib::Button
{
    bool is_activated = false;
    int seq_num = 0;
    static constexpr int size = 100;

    Cell(Point xy, Graph_lib::Callback cb)
        : Graph_lib::Button(xy,size,size,"",cb){};

    void attach (Graph_lib::Window& win) override;
    void activate(int n){
        is_activated = true;
        seq_num = n;
        pw->color(FL_RED);
    }
    void hide_color(){pw->color(FL_BACKGROUND_COLOR);}
};
void Cell::attach (Graph_lib::Window& win)
{
    Button::attach(win);
}

class GameTable: Graph_lib::Window{
public:
    static constexpr int N = 4;
    GameTable(Point xy)
        :Graph_lib::Window(xy,height,width,"Miami")
    {
        //TODO: Сделать outbox с обратным отсчетом до того, как выведутся клетки \ DONE

        countdown_box.set_fill_color(FL_CYAN);
        countdown_text.set_font(FL_TIMES_ITALIC);
        countdown_text.set_font_size(40);
        result_box.set_fill_color(FL_CYAN);
        score_box.set_fill_color(FL_CYAN);
        result_text.set_font(FL_TIMES_ITALIC);
        result_text.set_font_size(40);
        score_text.set_font(FL_TIMES_ITALIC);
        score_text.set_font_size(40);

        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                cells.push_back(new Cell{ Point{margin + j*Cell::size,
                                                margin + (N-1 - i)*Cell::size}, cb});
                attach(cells[cells.size()-1]);
            }
        }
        start_game();

    };

private:
    static constexpr int margin = 30;
    static constexpr int width = 600;
    static constexpr int height = 600;
    static constexpr int dt = 1000;
    // count of activated cells
    int num = 1;
    // короче это итератор каким по счету чел нажал кнопку
    int i = 1;
    // короче это счет чела
    int score = 0;

    Graph_lib::Vector_ref<Cell> cells;
    Graph_lib::Rectangle countdown_box = {Point(150,200),300, 80};
    Graph_lib::Text countdown_text = {Point(100+100,200+50), ""};
    Graph_lib::Rectangle result_box = {Point(150,120),300, 80};
    Graph_lib::Text result_text = {Point(150+80,120+50), "NONE"};
    Graph_lib::Button play_again_button =  {Point(150,280),300,80, "Play again!", cb_again};
    Graph_lib::Button continue_button = {Point(150,280),300,80, "Continue!", cb_again};
    Graph_lib::Button quit = {Point(150,360),300,80, "Quit!", cb_quit};
    Graph_lib::Rectangle score_box = {Point(150,200),300, 80};
    Graph_lib::Text score_text = {Point(150+80,200+50), "SCORE:" + to_string(score)};

    vector <int> order;

    Cell& at(Point p)
    {
        int i = N-1 - (p.y - margin) / Cell::size;
        int j = (p.x - margin) / Cell::size;
        return cells[i*N + j];
    }

    void start_game()
    {
        countdown();
        activation();
    }

    void countdown()
    {
        attach(countdown_box);
        attach(countdown_text);
        for (int i = 3; i > -1; i--)
        {
            countdown_text.set_label("Start in " + to_string(i) + " sec");
            flush();
            Sleep(dt);
        }
        detach(countdown_box);
        detach(countdown_text);
        flush();
        Sleep(dt);
    }

    void activation()
    {
        for (int i = 1;i <= num;++i)
        {
            int rand_index = rand()%cells.size();
            while (count( order.begin(), order.end(), rand_index) != 0)
            {
                rand_index = rand()%cells.size();
            }
            order.push_back(rand_index);
            cells[rand_index].activate(i);
            flush();
            Sleep(dt);
            cells[rand_index].hide_color();
            flush();
            Sleep(dt);
        }
    }

    void start_again()
    {
        detach(result_box);
        detach(result_text);
        detach(play_again_button);
        detach(quit);
        detach(score_box);
        detach(score_text);

        for (int i = 0; i < cells.size(); i++)
            cells[i].is_activated = false;

        i = 1;
        order.clear();

        start_game();
    }

    void exit()
    {
        hide();
    }

    static void cb(Graph_lib::Address wid, Graph_lib::Address pw)
    {
        Graph_lib::reference_to<GameTable> (pw).wait(wid);
    }
    static void cb_again(Graph_lib::Address wid, Graph_lib::Address pw)
    {
        Graph_lib::reference_to<GameTable> (pw).start_again();
    }
    static void cb_quit(Graph_lib::Address wid, Graph_lib::Address pw)
    {
        Graph_lib::reference_to<GameTable> (pw).exit();
    }

    void wait(Graph_lib::Address wid)
    {
        Fl_Widget& w = Graph_lib::reference_to<Fl_Widget>(wid);
        Cell& c = at (Point{w.x(), w.y()});

        string play_again_text = "Play again!";
        // TODO: Сделать вывод победы \ DONE
        if (c.seq_num == i)
        {
            i++;
            if (i > num)
            {
                score_text.set_label("SCORE:" + to_string(num));
                result_text.set_label("U RIGHT");
                score += 1;
                num += 1;
            }
            else
                return;
        }
        //TODO: Сделать кнопку назад при проигрыше \ DONE
        else
        {
            score = 0;
            result_text.set_label("U LOST");
            num = 1;       
        }

        attach(play_again_button);
        attach(result_box);
        attach(result_text);
        attach(quit);
        attach(score_box);
        attach(score_text);

        put_on_top(play_again_button);
        put_on_top(quit);

        flush();
    }
};

int main()
{
    GameTable board{ Point{100, 100} };
    Graph_lib::gui_main();
}
