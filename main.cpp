#include <iostream>
#include <ctime>
#include "dungeongen.h"

using namespace std;

int main(int argc, char ** argv)
{
    DungeonMap test(6, 10, 15);

    test.generate(time(NULL));

    cout << test.as_str() << endl;

    return 0;
}