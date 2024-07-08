#include <iostream>
#include "libraries/imagelib.hpp"

int main()
{
    Image startImage("raw_images/test_image.raw");
    startImage.ExportGaussian5x5("generated/gaussian_prefilter.raw");

    Image blurredImage("generated/gaussian_prefilter.raw");
    blurredImage.ExportSobelMagnitude("generated/sobel_magnitude.raw", 10, 17);

    Image sobelImage("generated/sobel_magnitude.raw");
    // sobelImage.exportVoronoi()

    return 0;
}