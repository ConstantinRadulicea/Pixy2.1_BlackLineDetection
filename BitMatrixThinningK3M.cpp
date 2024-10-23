#include <iostream>
#include <vector>

using namespace std;

// Directions to access 8 neighbors of a pixel (N, NE, E, SE, S, SW, W, NW)
const int dx[] = { -1, -1, 0, 1, 1, 1, 0, -1 };
const int dy[] = { 0, 1, 1, 1, 0, -1, -1, -1 };

// Check if a pixel is within the bounds of the image
bool isInside(int x, int y, int rows, int cols) {
    return x >= 0 && x < rows && y >= 0 && y < cols;
}

// Count the number of neighboring 1's (foreground pixels)
int countNeighbors(const vector<vector<int>>& img, int x, int y) {
    int count = 0;
    for (int i = 0; i < 8; i++) {
        int nx = x + dx[i];
        int ny = y + dy[i];
        if (isInside(nx, ny, img.size(), img[0].size()) && img[nx][ny] == 1) {
            count++;
        }
    }
    return count;
}

// Count the number of 0-1 transitions in the 8-neighborhood (for connectivity)
int countTransitions(const vector<vector<int>>& img, int x, int y) {
    int transitions = 0;
    for (int i = 0; i < 8; i++) {
        int nx = x + dx[i];
        int ny = y + dy[i];
        int nnx = x + dx[(i + 1) % 8];
        int nny = y + dy[(i + 1) % 8];
        if (isInside(nx, ny, img.size(), img[0].size()) &&
            isInside(nnx, nny, img.size(), img[0].size()) &&
            img[nx][ny] == 0 && img[nnx][nny] == 1) {
            transitions++;
        }
    }
    return transitions;
}

// Apply the K3M thinning algorithm to the image
void thinningK3M(vector<vector<int>>& img) {
    bool pixelsRemoved;

    do {
        pixelsRemoved = false;
        vector<pair<int, int>> toRemove;

        // Loop through all pixels in the image
        for (int i = 1; i < img.size() - 1; i++) {
            for (int j = 1; j < img[0].size() - 1; j++) {
                if (img[i][j] == 1) {
                    int neighbors = countNeighbors(img, i, j);
                    int transitions = countTransitions(img, i, j);

                    // Check if the pixel satisfies the K3M conditions for removal
                    if (neighbors >= 2 && neighbors <= 6 && transitions == 1) {
                        if (img[i - 1][j] == 0 || img[i][j + 1] == 0 || img[i + 1][j] == 0) {
                            toRemove.push_back({ i, j });
                        }
                    }
                }
            }
        }

        // Remove the pixels that were marked for deletion
        for (auto& p : toRemove) {
            img[p.first][p.second] = 0;
            pixelsRemoved = true;
        }

    } while (pixelsRemoved);  // Repeat until no more pixels are removed
}

// Print the binary image
void printImage(const vector<vector<int>>& img) {
    for (const auto& row : img) {
        for (int pixel : row) {
            cout << (pixel ? "1 " : "0 ");
        }
        cout << endl;
    }
}

int main23dz() {
    // Example binary image (object pixels = 1, background pixels = 0)
    //vector<vector<int>> img = {
    //    {0, 0, 0, 0, 0, 0, 0},
    //    {0, 0, 0, 0, 0, 0, 0},
    //    {0, 0, 0, 0, 0, 0, 0},
    //    {0, 0, 0, 0, 0, 0, 0},
    //    {0, 0, 0, 0, 0, 0, 0},
    //};

    //vector<vector<int>> img = {
    //{0, 0, 0, 0, 1, 1, 0},
    //{0, 0, 0, 1, 1, 0, 0},
    //{0, 0, 1, 1, 0, 0, 0},
    //{0, 1, 1, 0, 0, 0, 0},
    //{1, 1, 0, 0, 0, 0, 0},
    //};

        vector<vector<int>> img = {
        {0, 0, 1, 1, 0, 0, 0},
        {0, 0, 1, 1, 0, 0, 0},
        {0, 0, 1, 1, 0, 0, 0},
        {0, 0, 1, 1, 0, 0, 0},
        {0, 0, 1, 1, 0, 0, 0},
    };

    cout << "Original image:\n";
    printImage(img);

    thinningK3M(img);

    cout << "\nThinned image (K3M):\n";
    printImage(img);

    return 0;
}
