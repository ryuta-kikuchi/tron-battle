#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include <memory>
#include <random>
#include <chrono>

std::random_device rnd;
std::mt19937 mt(rnd());

constexpr int INF = 1000000000LL;

using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

class GameState
{
private:
    static constexpr int dx_[4] = {1, -1, 0, 0};
    static constexpr int dy_[4] = {0, 0, 1, -1};
    static constexpr int MAX_X_ = 30;
    static constexpr int MAX_Y_ = 20;

    const int n_;
    const int p_;
    vector<vector<int>> field_;
    vector<pair<int, int>> player_positions_;
    set<int> eliminated_players_;
    int current_player_;

    bool isOutOfField(const int x, const int y) const
    {
        if (x < 0 || MAX_X_ <= x || y < 0 || MAX_Y_ <= y)
        {
            return true;
        }
        return false;
    }

    bool isEliminated(const int player) const
    {
        return eliminated_players_.count(player) > 0;
    }

    void eliminatePlayer(const int player_to_eliminate)
    {
        eliminated_players_.insert(player_to_eliminate);
        player_positions_[player_to_eliminate] = {-1, -1};

        for (int i = 0; i <= MAX_X_; i++)
        {
            for (int j = 0; j <= MAX_Y_; j++)
            {
                if (field_[i][j] == player_to_eliminate)
                {
                    field_[i][j] = -1;
                }
            }
        }
    }

public:
    GameState(
        const int n,
        const int p,
        const int current_player,
        const vector<pair<int, int>> &initial_player_positions = vector<pair<int, int>>(4),
        const vector<vector<int>> &field = vector<vector<int>>(MAX_X_ + 1, vector<int>(MAX_Y_ + 1, -1))) : n_(n),
                                                                                                           p_(p),
                                                                                                           current_player_(current_player),
                                                                                                           player_positions_(initial_player_positions),
                                                                                                           field_(field)
    {
        for (int i = 0; i < n; i++)
        {
            const int x = player_positions_[i].first;
            const int y = player_positions_[i].second;
            field_[x][y] = i;
        }
    }

    vector<pair<int, int>> getLegalActions() const
    {
        vector<pair<int, int>> legal_actions;

        const int current_x = player_positions_[current_player_].first;
        const int current_y = player_positions_[current_player_].second;

        for (int i = 0; i < 4; i++)
        {
            const int next_x = current_x + dx_[i];
            const int next_y = current_y + dy_[i];

            if (isEliminated(current_player_) || isOutOfField(next_x, next_y) || field_[next_x][next_y] != -1)
            {
                continue;
            }

            legal_actions.push_back({next_x, next_y});
        }

        if (legal_actions.empty())
        {
            legal_actions.push_back({-1, -1});
        }

        return legal_actions;
    };

    void advance(const int x, const int y)
    {
        if (!isEliminated(current_player_))
        {
            if (x == -1 && y == -1)
            {
                eliminatePlayer(current_player_);
            }
            else
            {
                field_[x][y] = current_player_;
                player_positions_[current_player_] = {x, y};
            }
        }

        current_player_ += 1;
        current_player_ %= n_;
    }

    bool isWin()
    {
        return eliminated_players_.size() == n_ - 1 && eliminated_players_.count(p_) == 0;
    }

    bool isLose()
    {
        return eliminated_players_.count(p_) == 1;
    }
};

string movementToString(const int current_x, const int current_y, const int next_x, const int next_y)
{
    if (next_x == -1 && next_y == -1)
    {
        return "CAN'T MOVE";
    }

    static constexpr int dx_[4] = {1, -1, 0, 0};
    static constexpr int dy_[4] = {0, 0, 1, -1};
    static constexpr char directions[4][10] = {
        "RIGHT",
        "LEFT",
        "DOWN",
        "UP",
    };

    for (int i = 0; i < 4; i++)
    {
        if (next_x == current_x + dx_[i] && next_y == current_y + dy_[i])
        {
            return string(directions[i]);
        }
    }
    return "ERROR";
}

pair<int, int> randomAction(shared_ptr<GameState> state)
{
    auto legal_actions = state->getLegalActions();
    return legal_actions[mt() % (legal_actions.size())];
}

double playout(shared_ptr<GameState> state)
{
    if (state->isLose())
        return 0;
    if (state->isWin())
        return 1;
    auto selected_action = randomAction(state);
    state->advance(selected_action.first, selected_action.second);
    return playout(state);
}

pair<int, int> primitiveMontecarloAction(shared_ptr<GameState> state, int num_of_playouts)
{
    auto legal_actions = state->getLegalActions();
    double best_value = -INF;
    int best_i = -1;
    for (int i = 0; i < legal_actions.size(); i++)
    {
        double value = 0;
        for (int j = 0; j < num_of_playouts; j++)
        {
            shared_ptr<GameState> next_state = make_shared<GameState>(*state);
            next_state->advance(legal_actions[i].first, legal_actions[i].second);
            value += playout(next_state);
        }
        if (value > best_value)
        {
            best_i = i;
            best_value = value;
        }
    }
    return legal_actions[best_i];
}

int main()
{

    bool initialized = false;
    shared_ptr<GameState> game_state = nullptr;
    vector<pair<int, int>> player_movements(4);
    vector<pair<int, int>> initial_player_positions(4);

    // game loop
    while (1)
    {

        int n; // total number of players (2 to 4).
        int p; // your player number (0 to 3).
        cin >> n >> p;
        cin.ignore();

        for (int i = 0; i < n; i++)
        {
            int x0; // starting X coordinate of lightcycle (or -1)
            int y0; // starting Y coordinate of lightcycle (or -1)
            int x1; // starting X coordinate of lightcycle (can be the same as X0 if you play before this player)
            int y1; // starting Y coordinate of lightcycle (can be the same as Y0 if you play before this player)
            cin >> x0 >> y0 >> x1 >> y1;
            cin.ignore();

            initial_player_positions[i] = {x0, y0};
            player_movements[i] = {x1, y1};
        }

        if (!initialized)
        {
            game_state = make_shared<GameState>(n, p, 0, initial_player_positions);
            for (int i = 0; i < p; i++)
            {
                game_state->advance(player_movements[i].first, player_movements[i].second);
            }

            initialized = true;
        }
        else
        {
            for (int i = p + 1; i < n; i++)
            {
                game_state->advance(player_movements[i].first, player_movements[i].second);
            }
            for (int i = 0; i < p; i++)
            {
                game_state->advance(player_movements[i].first, player_movements[i].second);
            }
        }

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;

        auto best_action = primitiveMontecarloAction(game_state, 10);
        game_state->advance(best_action.first, best_action.second);

        auto action = movementToString(player_movements[p].first, player_movements[p].second, best_action.first, best_action.second);
        // cerr << player_movements[p].first << " " << player_movements[p].second << " " << legal_actions[0].first << " " << legal_actions[0].second << endl;
        cout << action << endl;
    }
}