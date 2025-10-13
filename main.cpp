#include <iostream>
#include <ctime>
#include "dungeongen.h"

using namespace std;

int main(int argc, char ** argv)
{
    DungeonMap test(30, 4, 10);

    test.generate(time(NULL));

    cout << test.as_tile_str() << endl;

    return 0;
}