#include <iostream>
#include <ctime>
#include <cstring>
#include "bitmap_image.hpp"

const unsigned char COLOR_BLACK = 0;
const unsigned char COLOR_WHITE = 255;

const unsigned char ALIVE = 1;
const unsigned char DEAD = 0;

struct Raster {
    Raster(int w, int h) : width(w), height(h)
    {
        data = new int[width*height];
    }

    Raster(int w, int h, float seedProbability) : width(w), height(h)
    {
        data = new int[size];

        for (int i = 0; i < size; i++) {
            float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
            if (random < seedProbability) {
                data[i] = ALIVE;
            }
        }
    }

    Raster(const std::string &filename)
    {
        bitmap_image image(filename);

        if (!image)
        {
            std::cerr << "Could not open bitmap!" << std::endl;
        }

        height = image.height();
        width = image.width();

        data = new int[width*height];

        unsigned char red;
        unsigned char green;
        unsigned char blue;

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                image.get_pixel(x, y, red, green, blue);
                if (red == COLOR_BLACK && green == COLOR_BLACK && blue == COLOR_BLACK) {
                    data[index(x,y)] = ALIVE;
                }
            }
        }
    }

    void save(const std::string &filename)
    {
        bitmap_image image(width, height);

        image.set_all_channels(COLOR_WHITE, COLOR_WHITE, COLOR_WHITE);

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                unsigned char color = (data[index(x,y)] == ALIVE) ? COLOR_BLACK : COLOR_WHITE;
                image.set_pixel(x, y, color, color, color);
            }
        }

        image.save_image(filename);
    }

    bool inBounds(const int &x, const int &y) const {
        if (x < 0 || y < 0 || x > width-1 || y > height-1) {
            return false;
        }
        return true;
    }

    ~Raster()
    {
        delete[] data;
    }

    size_t index(int x, int y) const { return x + width * y; }
    size_t x(int index) const { return index % width; }
    size_t y(int index) const { return (index - x(index)) / width; }

    int width;
    int height;
    int size = width*height;
    int* data;
};

// This struct parses all necessary command line parameters. It is already complete and doesn't have to be modified. However - feel free to add support for additional arguments if you like.
struct CommandLineParameter
{
    CommandLineParameter(int argc, char* argv[])
        : width(0)
        , height(0)
        , invasionFactor(0)
        , isTorus(false)
        , maxIterations(20)
    {
        if (argc % 2 == 0)
        {
            std::cerr << "Missing value for " << argv[argc - 1] << std::endl;
            argc--;
        }

        for (int i = 1; i < argc; i += 2)
        {
            if (!strcmp(argv[i], "-w"))
            {
                width = atoi(argv[i + 1]);
            }
            else if (!strcmp(argv[i], "-h"))
            {
                height = atoi(argv[i + 1]);
            }
            else if (!strcmp(argv[i], "-s"))
            {
                seedProbability = atof(argv[i + 1]);
            }
            else if (!strcmp(argv[i], "-p"))
            {
                patternFilename = argv[i + 1];
            }
            else if (!strcmp(argv[i], "-o"))
            {
                outputDirectory = argv[i + 1];
            }
            else if (!strcmp(argv[i], "-iv"))
            {
                invasionFactor = atof(argv[i + 1]);
            }
            else if (!strcmp(argv[i], "-t"))
            {
                isTorus = strcmp(argv[i + 1], "0") != 0;
            }
            else if (!strcmp(argv[i], "-i"))
            {
                maxIterations = atoi(argv[i + 1]);
            }
        }

        if ((width != 0 || height != 0) && !patternFilename.empty())
        {
            std::cout << "Width and height are ignored, because pattern is defined." << std::endl;
        }

        if (width < 0 || height < 0)
        {
            std::cerr << "Width or height has a invalid value." << std::endl;
            width = 0;
            height = 0;
        }
    }

    int width;
    int height;
    float seedProbability;
    std::string patternFilename;
    std::string outputDirectory;
    float invasionFactor;
    bool isTorus;
    int maxIterations;
};

int neighborValue(const Raster &raster, int x, int y, bool isTorus)
{
    int numNeighbours = 0;

    for (int yy = y-1; yy <= y+1; yy++) {
        for (int xx = x-1; xx <= x+1; xx++) {
            if (xx == x && yy == y) {
                continue; // not a neighbour
            }

            bool inBounds = raster.inBounds(xx,yy);

            if (!isTorus && !inBounds) {
                continue; // i.e. dead
            }

            int actualX = xx;
            int actualY = yy;

            if (!inBounds) {
                // transform to torus
                actualX = (xx + raster.width) % raster.width;
                actualY = (yy + raster.height) % raster.height;
            }

            const int currentCellState = raster.data[raster.index(actualX, actualY)];

            if (currentCellState == ALIVE) {
                numNeighbours++;
            }
        }
    }

    const int cellState = raster.data[raster.index(x,y)];

    // apply rules
    int newState = DEAD;
    if (cellState == DEAD && numNeighbours == 3) {
        newState = ALIVE;
    } else if (cellState == ALIVE && (numNeighbours == 2 || numNeighbours == 3)) {
        newState = ALIVE;
    }

    return newState;
}

void simulateInvasion(Raster &raster, float invasionFactor)
{
    if (invasionFactor <= 0)
    {
        return;
    }

    for (int y = 0; y < raster.height; ++y) {
        for (int x = 0; x < raster.width; ++x) {
            float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
            if (random < invasionFactor) {
                int index = raster.index(x,y);
                // flip cell state
                raster.data[index] = (raster.data[index] + 1) % 2;
            }
        }
    }
}

void simulateNextState(Raster &raster, bool isTorus)
{
    const int width = raster.width;
    const int height = raster.height;

    int* data = new int[width*height];

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int index = raster.index(x,y);
            data[index] = neighborValue(raster, x, y, isTorus);
        }
    }

    delete[] raster.data;

    raster.data = data;
}

int main(int argc, char* argv[])
{
    Raster* raster = nullptr;
    CommandLineParameter cmd(argc, argv);

    srand(time(NULL));

    if (!cmd.patternFilename.empty())
    {
        raster = new Raster(cmd.patternFilename);
    }
    else
    {
        raster = new Raster(cmd.width, cmd.height, cmd.seedProbability);
    }

    for (int iteration = 0; iteration <= cmd.maxIterations; iteration++)
    {
        raster->save(cmd.outputDirectory + "game_of_life_" + std::to_string(iteration) + ".bmp");
        simulateInvasion(*raster, cmd.invasionFactor);
        simulateNextState(*raster, cmd.isTorus);
    }

    delete raster;

    return 0;
}
