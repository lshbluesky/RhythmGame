#include<iostream>
#include<vector>

using namespace std;
#define TABLE_X 18
#define TABLE_Y 20

class MainMenu {
public:
    MainMenu() {
        cout << "\n\n\n\n";
        cout << "\t\t"; cout << "      rrrrrrrrrrr      dddddddddddd\n";
        cout << "\t\t"; cout << "      rr        rr     dd          dd\n";
        cout << "\t\t"; cout << "      rr         rr    dd            dd\n";
        cout << "\t\t"; cout << "      rr         rr    dd             dd\n";
        cout << "\t\t"; cout << "      rr         rr    dd             dd\n";
        cout << "\t\t"; cout << "      rr        rr     dd             dd\n";
        cout << "\t\t"; cout << "      rrrrrrrrrr       dd             dd\n";
        cout << "\t\t"; cout << "      rr       rr      dd            dd\n";
        cout << "\t\t"; cout << "      rr         rr    dd          dd\n";
        cout << "\t\t"; cout << "      rr           rr  ddccccccccdd\n\n";
        cout << "\t\t"; cout << "둠칫~둠칫 아무키나 누르고 너의 리듬을 보여줘!\n\n\n\n";
        getchar();
        system("cls");
    }
};

class GameTable {
private:
    int x;
    int y;
    vector<vector<int>> table;
public:
    GameTable(int x, int y) {
        this->x = x;
        this->y = y;
        for (int i = 0; i < y; i++) {
            vector<int> temp;
            for (int j = 0; j < x; j++) {
                temp.push_back(0);
            }
            table.push_back(temp);
        }

        for (int i = 0; i < x; i++) {
            table[0][i] = 1;
            table[y - 1][i] = 1;
        }
        for (int i = 1; i < y - 1; i++) {
            table[i][0] = 1;
            table[i][x - 1] = 1;
        }
    }
    void DrawGameTable() {
        for (int i = 0; i < y; i++) {
            for (int j = 0; j < x; j++) {
                if (table[i][j] == 1) cout << "=";
                else cout << " ";
            }
            cout << "\n";
        }
    }

};

int main(void) {
    system("mode con cols=100 lines=40 | title RD");
    GameTable gt(TABLE_X, TABLE_Y);
    MainMenu();
    gt.DrawGameTable();
    getchar();
    return 0;
}