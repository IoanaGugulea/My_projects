#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <climits>

using namespace std;

class Point {
public:
    int x, y, score;
};

class GoldenPoint : public Point {
};

class SilverPoint : public Point {
public:
    int score = 0;
};

class Tile {
public:
    char tid;
    int cost;
    int quantity;
    int directions[4][2]; // maximum possible directions
};

bool isWithinBounds(int x, int y, int W, int H) {
    return x >= 0 && x < W && y >= 0 && y < H;
}

void initializeTileDirections(Tile& tile) {
    switch (tile.tid) {
    case '3':
        tile.directions[0][0] = 0; tile.directions[0][1] = 1; // right
        break;
    case '5':
        tile.directions[0][0] = 1; tile.directions[0][1] = 0; // down
        tile.directions[1][0] = 0; tile.directions[1][1] = 1; // right
        break;
    case '6':
        tile.directions[0][0] = 0; tile.directions[0][1] = -1; // left
        tile.directions[1][0] = 1; tile.directions[1][1] = 0; // down
        break;
    case '7':
        tile.directions[0][0] = 0; tile.directions[0][1] = 1; // right
        tile.directions[1][0] = 0; tile.directions[1][1] = -1; // left
        tile.directions[2][0] = 1; tile.directions[2][1] = 0; // down
        break;
    case '9':
        tile.directions[0][0] = -1; tile.directions[0][1] = 0; // up
        tile.directions[1][0] = 1; tile.directions[1][1] = 0; // down
        break;
    case 'A':
        tile.directions[0][0] = 0; tile.directions[0][1] = -1; // left
        tile.directions[1][0] = -1; tile.directions[1][1] = 0; // up
        break;
    case 'B':
        tile.directions[0][0] = 0; tile.directions[0][1] = 1; // right
        tile.directions[1][0] = 0; tile.directions[1][1] = -1; // left
        tile.directions[2][0] = -1; tile.directions[2][1] = 0; // up
        break;
    case 'C':
        tile.directions[0][0] = -1; tile.directions[0][1] = 0; // up
        tile.directions[1][0] = 1; tile.directions[1][1] = 0; // down
        break;
    case 'D':
        tile.directions[0][0] = -1; tile.directions[0][1] = 0; // up
        tile.directions[1][0] = 0; tile.directions[1][1] = 1; // right
        tile.directions[2][0] = 1; tile.directions[2][1] = 0; // down
        break;
    case 'E':
        tile.directions[0][0] = 0; tile.directions[0][1] = -1; // left
        tile.directions[1][0] = 0; tile.directions[1][1] = 1; // right
        tile.directions[2][0] = -1; tile.directions[2][1] = 0; // up
        tile.directions[3][0] = 1; tile.directions[3][1] = 0; // down
        break;
    case 'F':
        tile.directions[0][0] = 0; tile.directions[0][1] = -1; // left
        tile.directions[1][0] = 0; tile.directions[1][1] = 1; // right
        tile.directions[2][0] = -1; tile.directions[2][1] = 0; // up
        tile.directions[3][0] = 1; tile.directions[3][1] = 0; // down
        break;
    }
}

// Custom compare function for priority queue
class CompareCost {
public:
    bool operator()(pair<int, Point>& p1, pair<int, Point>& p2) {
        return p1.first > p2.first; // Min heap based on cost
    }
};

// Dijkstra's algorithm to find the most efficient route
int findRoute(const Point& start, const Point& end, int W, int H, char** grid, Point* goldenPoints, int GN, Point* silverPoints, int SM, Tile* tiles, int TL) {
    const int INF = INT_MAX;
    int result = 0;

    vector<vector<int>> cost(H, vector<int>(W, INF));
    vector<vector<int>> score(H, vector<int>(W, 0));

    priority_queue<pair<int, Point>, vector<pair<int, Point>>, CompareCost> pq;

    cost[start.y][start.x] = 0;
    pq.push({ 0, start });

    while (!pq.empty()) {
        int cur_cost = pq.top().first;
        Point cur_point = pq.top().second;
        pq.pop();

        if (cur_cost > cost[cur_point.y][cur_point.x]) {
            continue;
        }

        if (cur_point.x == end.x && cur_point.y == end.y) {
            result = max(result, score[cur_point.y][cur_point.x]);
        }

        for (int i = 0; i < 4; ++i) {
            int new_x = cur_point.x + tiles[i].directions[i][0];
            int new_y = cur_point.y + tiles[i].directions[i][1];

            if (isWithinBounds(new_x, new_y, W, H)) {
                int new_cost = cur_cost + tiles[i].cost;
                if (grid[new_y][new_x] != '.' || new_cost >= cost[new_y][new_x] || new_cost >= INF) {
                    continue;
                }

                cost[new_y][new_x] = new_cost;
                score[new_y][new_x] = score[cur_point.y][cur_point.x];

                // Accumulate score based on neighboring silver points
                for (int j = 0; j < SM; ++j) {
                    if (isWithinBounds(silverPoints[j].x, silverPoints[j].y, W, H) &&
                        abs(silverPoints[j].x - new_x) <= 1 && abs(silverPoints[j].y - new_y) <= 1) {
                        score[new_y][new_x] += silverPoints[j].score;
                    }
                }

                if (grid[new_y][new_x] != '.') {
                    cost[new_y][new_x] *= 2;
                    score[new_y][new_x] *= 2;
                }

                pq.push({ new_cost, {new_x, new_y} });
            }
        }
    }

    return max(0, result);
}




void read_matrix(const string& file_name, int& W, int& H, int& GN, int& SM, int& TL,
    GoldenPoint* golden_points, SilverPoint* silver_points, Tile* tiles) {
    ifstream file(file_name);
    file >> W >> H >> GN >> SM >> TL;

    for (int i = 0; i < GN; ++i) {
        file >> golden_points[i].x >> golden_points[i].y;
    }

    for (int i = 0; i < SM; ++i) {
        file >> silver_points[i].x >> silver_points[i].y >> silver_points[i].score;
    }

    for (int i = 0; i < TL; ++i) {
        file >> tiles[i].tid >> tiles[i].cost >> tiles[i].quantity;
    }

    file.close();
}

void outputGrid(ofstream& output_file, int W, int H, char** grid) {
    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
            output_file << grid[i][j];
        }
        output_file << endl;
    }
}

int main()
{
    int W, H, GN, SM, TL;
    string file_name = "00-trailer.txt";
    ifstream count_file(file_name);
    count_file >> W >> H >> GN >> SM >> TL;
    count_file.close();

    GoldenPoint* golden_points = new GoldenPoint[GN];
    SilverPoint* silver_points = new SilverPoint[SM];
    Tile* tiles = new Tile[TL];

    read_matrix(file_name, W, H, GN, SM, TL, golden_points, silver_points, tiles);

    char** grid = new char* [H];
    for (int i = 0; i < H; ++i) {
        grid[i] = new char[W];
        for (int j = 0; j < W; ++j) {
            grid[i][j] = '.';
        }
    }

    for (int i = 0; i < GN; ++i) {
        grid[golden_points[i].y][golden_points[i].x] = 'G';
    }
    for (int i = 0; i < SM; ++i) {
        grid[silver_points[i].y][silver_points[i].x] = 'S';
    }

    int total_score = 0;
    for (int i = 0; i < GN; ++i) {
        for (int j = i + 1; j < GN; ++j) {
            int score = findRoute(golden_points[i], golden_points[j], W, H, grid, golden_points, GN, silver_points, SM, tiles, TL);
            if (score < 0) {
                score = 0;
            }
            total_score += score;
        }
    }

    ofstream output_file("00-trailer-output.txt");

    outputGrid(output_file, W, H, grid);

    for (int i = 0; i < TL; ++i) {
        // Access x and y coordinates of tiles correctly
        output_file << tiles[i].tid << " " << tiles[i].directions[0][1] << " " << tiles[i].directions[0][0] << endl;
    }

    output_file.close();

    delete[] golden_points;
    delete[] silver_points;
    delete[] tiles;
    for (int i = 0; i < H; ++i) {
        delete[] grid[i];
    }
    delete[] grid;
    return 0;
}