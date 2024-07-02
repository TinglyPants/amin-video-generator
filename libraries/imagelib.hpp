#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>

void getHeaderChunk(std::ifstream &file, uint32_t &output);
void getPixelData(std::ifstream &file, std::vector<uint8_t> &pixelVector, uint32_t width, uint32_t height);
char getEdgeDirection(int GY, int GX);

class Image
{
public:
    uint32_t width = 0;
    uint32_t height = 0;
    std::vector<uint8_t> pixels;

    Image(std::string filename)
    {
        std::ifstream rawFile(filename, std::ios::binary);

        // Gather width and height
        getHeaderChunk(rawFile, width);
        getHeaderChunk(rawFile, height);

        // Set pixel vector to correct size
        pixels.resize(width * height * 3);
        getPixelData(rawFile, pixels, width, height);

        rawFile.close();
    }

    uint8_t getPixelRed(uint32_t x, uint32_t y)
    {
        return pixels[((width * y) + x) * 3];
    }

    uint8_t getPixelGreen(uint32_t x, uint32_t y)
    {
        return pixels[(((width * y) + x) * 3) + 1];
    }

    uint8_t getPixelBlue(uint32_t x, uint32_t y)
    {
        return pixels[(((width * y) + x) * 3) + 2];
    }

    uint8_t getPixelValue(uint32_t x, uint32_t y)
    {
        // since uint32_T cannot be less than 0, we dont need to check that one.
        if (x >= width || y >= height)
        {
            return 0;
        }
        uint16_t total = 0;
        uint8_t redValue = getPixelRed(x, y);
        uint8_t greenValue = getPixelGreen(x, y);
        uint8_t blueValue = getPixelBlue(x, y);

        total = redValue + blueValue + greenValue;
        total /= 3;

        return static_cast<uint8_t>(total);
    }

    int getSobelGX(uint32_t x, uint32_t y)
    {
        int kernel[3][3] = {{1, 0, -1},
                            {2, 0, -2},
                            {1, 0, -1}};

        int lowerBound = -1;
        int upperBound = 1;

        int total = 0;

        for (int j = lowerBound; j <= upperBound; j++)
        {
            for (int i = lowerBound; i <= upperBound; i++)
            {
                int kernelValue = kernel[j + 1][i + 1];
                uint8_t pixelValue = getPixelValue(x + i, y + j);

                total += kernelValue * pixelValue;
            }
        }

        return total / 4;
    }

    int getSobelGY(uint32_t x, uint32_t y)
    {
        int kernel[3][3] = {{1, 2, 1},
                            {0, 0, 0},
                            {-1, -2, -1}};

        int lowerBound = -1;
        int upperBound = 1;

        int total = 0;

        for (int j = lowerBound; j <= upperBound; j++)
        {
            for (int i = lowerBound; i <= upperBound; i++)
            {
                int kernelValue = kernel[j + 1][i + 1];
                uint8_t pixelValue = getPixelValue(x + i, y + j);

                total += kernelValue * pixelValue;
            }
        }

        return total / 4;
    }

    uint16_t getGaussianBlur3x3Value(uint32_t x, uint32_t y)
    {
        uint8_t kernel[3][3] = {{1, 2, 1},
                                {2, 4, 2},
                                {1, 2, 1}};

        int lowerBound = -1;
        int upperBound = 1;

        int total = 0;

        for (int j = lowerBound; j <= upperBound; j++)
        {
            for (int i = lowerBound; i <= upperBound; i++)
            {
                uint8_t kernelValue = kernel[j + 1][i + 1];
                uint8_t pixelValue = getPixelValue(x + i, y + j);

                total += kernelValue * pixelValue;
            }
        }

        total /= 16;

        return total;
    }

    uint16_t getGaussianBlur5x5Value(uint32_t x, uint32_t y)
    {
        uint8_t kernel[5][5] = {
            {1, 4, 7, 4, 1},
            {4, 16, 26, 16, 4},
            {7, 26, 41, 26, 7},
            {4, 16, 26, 16, 4},
            {1, 4, 7, 4, 1},
        };

        int lowerBound = -2;
        int upperBound = 2;

        int total = 0;

        for (int j = lowerBound; j <= upperBound; j++)
        {
            for (int i = lowerBound; i <= upperBound; i++)
            {
                uint8_t kernelValue = kernel[j + 2][i + 2];
                uint8_t pixelValue = getPixelValue(x + i, y + j);

                total += kernelValue * pixelValue;
            }
        }

        total /= 273;

        return total;
    }

    void getDistanceRotation(int &distance, int &rotation, uint32_t x, uint32_t y)
    {
        int upperBound = 1;
        int lowerBound = -1;
        bool found = false;
        int foundX = 0;
        int foundY = 0;
        double currentDistance = 0;

        while (!found)
        {
            double xDistance = width;
            double yDistance = height;
            currentDistance = std::sqrt(std::pow(xDistance, 2) + std::pow(yDistance, 2));

            // top row
            for (int i = lowerBound; i <= upperBound; i++)
            {
                if (getPixelValue(x + i, y + lowerBound) == 255)
                {
                    foundX = x + i;
                    foundY = y + lowerBound;
                    if (std::sqrt(std::pow(x - foundX, 2) + std::pow(y - foundY, 2)) < currentDistance)
                    {
                        currentDistance = std::sqrt(std::pow(x - foundX, 2) + std::pow(y - foundY, 2));
                        found = true;
                    }
                }
            }
            // bottom row
            for (int i = lowerBound; i <= upperBound; i++)
            {
                if (getPixelValue(x + i, y + upperBound) == 255)
                {
                    foundX = x + i;
                    foundY = y + upperBound;
                    if (std::sqrt(std::pow(x - foundX, 2) + std::pow(y - foundY, 2)) < currentDistance)
                    {
                        currentDistance = std::sqrt(std::pow(x - foundX, 2) + std::pow(y - foundY, 2));
                        found = true;
                    }
                }
            }
            // left column
            for (int i = lowerBound + 1; i <= upperBound - 1; i++)
            {
                if (getPixelValue(x + lowerBound, y + i) == 255)
                {
                    foundX = x + lowerBound;
                    foundY = y + i;
                    if (std::sqrt(std::pow(x - foundX, 2) + std::pow(y - foundY, 2)) < currentDistance)
                    {
                        currentDistance = std::sqrt(std::pow(x - foundX, 2) + std::pow(y - foundY, 2));
                        found = true;
                    }
                }
            }
            // right column
            for (int i = lowerBound + 1; i <= upperBound - 1; i++)
            {
                if (getPixelValue(x + upperBound, y + i) == 255)
                {
                    foundX = x + upperBound;
                    foundY = y + i;
                    if (std::sqrt(std::pow(x - foundX, 2) + std::pow(y - foundY, 2)) < currentDistance)
                    {
                        currentDistance = std::sqrt(std::pow(x - foundX, 2) + std::pow(y - foundY, 2));
                        found = true;
                    }
                }
            }

            lowerBound--;
            upperBound++;
        }

        distance = (int)currentDistance;

        // TODO rotation
        rotation = 0;
    }

    void ExportGaussian3x3(std::string outFilename)
    {
        std::ofstream outFile(outFilename, std::ios::binary);
        outFile.write(reinterpret_cast<char *>(&width), 4);
        outFile.write(reinterpret_cast<char *>(&height), 4);

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                char buffer[1] = {static_cast<char>(getGaussianBlur3x3Value(x, y))};
                outFile.write(buffer, 1);
                outFile.write(buffer, 1);
                outFile.write(buffer, 1);
            }
        }

        outFile.close();
    }

    void ExportGaussian5x5(std::string outFilename)
    {
        std::ofstream outFile(outFilename, std::ios::binary);
        outFile.write(reinterpret_cast<char *>(&width), 4);
        outFile.write(reinterpret_cast<char *>(&height), 4);

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                char buffer[1] = {static_cast<char>(getGaussianBlur5x5Value(x, y))};
                outFile.write(buffer, 1);
                outFile.write(buffer, 1);
                outFile.write(buffer, 1);
            }
        }

        outFile.close();
    }

    void ExportSobelMagnitude(std::string outFilename, uint8_t lowerThreshold, uint8_t upperThreshold)
    {
        std::ofstream outFile(outFilename, std::ios::binary);
        outFile.write(reinterpret_cast<char *>(&width), 4);
        outFile.write(reinterpret_cast<char *>(&height), 4);

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                uint8_t magnitude = (abs(getSobelGX(x, y)) + abs(getSobelGY(x, y))) / 2;
                char direction = getEdgeDirection(getSobelGY(x, y), getSobelGX(x, y));
                // hysteresis threshold
                if (magnitude < lowerThreshold)
                {
                    magnitude = 0;
                    direction = static_cast<char>(0x00);
                }
                else if (magnitude < upperThreshold)
                {
                    magnitude = 0;
                    direction = static_cast<char>(0x00);
                    for (int j = -1; j <= 1; j++)
                    {
                        for (int i = -1; i <= 1; i++)
                        {
                            // If connected to an edge, mark as an edge.
                            if ((abs(getSobelGX(x + i, y + j)) + abs(getSobelGY(x + i, y + j))) / 2 > upperThreshold)
                            {
                                magnitude = 0;
                                direction = getEdgeDirection(getSobelGY(x, y), getSobelGX(x, y));
                            }
                        }
                    }
                }

                char magnitudeBuffer[1] = {static_cast<char>(magnitude)};
                char directionBuffer[1] = {direction};
                outFile.write(magnitudeBuffer, 1);
                outFile.write(directionBuffer, 1);
                outFile.write(magnitudeBuffer, 1);
            }
        }

        outFile.close();
    }
};

void getHeaderChunk(std::ifstream &file, uint32_t &output)
{
    char buffer[4] = {0};
    file.read(buffer, 4);

    output |= static_cast<uint8_t>(buffer[3]) << 24;
    output |= static_cast<uint8_t>(buffer[2]) << 16;
    output |= static_cast<uint8_t>(buffer[1]) << 8;
    output |= static_cast<uint8_t>(buffer[0]);
}

void getPixelData(std::ifstream &file, std::vector<uint8_t> &pixelVector, uint32_t width, uint32_t height)
{
    for (int i = 0; i < height * width * 3; i++)
    {
        // read a byte, then add that byte to the pixel vector
        char buffer[1] = {0};
        file.read(buffer, 1);
        pixelVector[i] = static_cast<uint8_t>(buffer[0]);
    }
}

char getEdgeDirection(int GY, int GX)
{
    double directionDegrees = std::atan2(GY, GX) * 57.2957795131;
    if (directionDegrees < 0.0)
    {
        directionDegrees = 180.0 - abs(directionDegrees);
    }

    if (directionDegrees >= 0.0 && directionDegrees < 22.5)
    {
        // horizontal
        // std::cout << "Horizontal\n";
        return static_cast<char>(0x00);
    }
    else if (directionDegrees >= 22.5 && directionDegrees < 67.5)
    {
        // diagonal north east
        // std::cout << "northeast\n";
        return static_cast<char>(0x40);
    }
    else if (directionDegrees >= 67.5 && directionDegrees < 112.5)
    {
        // vertical
        // std::cout << "vertical\n";
        return static_cast<char>(0x80);
    }
    else if (directionDegrees >= 112.5 && directionDegrees < 157.5)
    {
        // diagonal north west
        // std::cout << "northwest\n";
        return static_cast<char>(0xC0);
    }
    else if (directionDegrees >= 157.5)
    {
        // horizontal
        // std::cout << "Horizontal\n";
        return static_cast<char>(0x00);
    }
    else
    {
        std::cout << "Something wrong: " << directionDegrees << "\n";
    }
}