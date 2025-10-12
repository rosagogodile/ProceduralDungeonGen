#include <iostream>
#include "dungeongen.h"

using namespace std;

int main(int argc, char ** argv)
{
    int width = 9;
    int height = 9;
    ByteMatrix2D test(width,height);

    int count = 0;

    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < height; ++j)
        {
            test.set(i, j, count);
            count++;
        }
    }

    cout << test.as_str("\t") << endl;

    return 0;
}