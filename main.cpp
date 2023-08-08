#include <filesystem>
#include "border.hpp"

namespace fs = std::filesystem;

int main(int argc, const char* argv[]) 
{
    const std::string keys = {        
        "{input i   |   |   Path to input file}"
        "{output o  |   |   Path to output file}"
        "{help h usage ? |  |   See usage}"
    };

    cv::CommandLineParser parser(argc, argv, keys);
    
    if (parser.has("help"))
    {
        parser.printMessage();
    }
    else if(parser.has("input") && parser.has("output"))
    {
        const std::string input_path = parser.get<std::string>("input");
        const std::string output_path = parser.get<std::string>("output");
        if (!fs::exists(input_path))
        {
            std::cerr << "Error: File [" << input_path << "] does not exist." << std::endl; 
            exit(EXIT_FAILURE);
        }
        
        // Create output directory in case it does not exist
        fs::path parent_path = fs::path(output_path).parent_path();
        std::cout << "Log: Parent path [" << parent_path << "]." << std::endl;

        std::error_code ec;
        fs::create_directories(parent_path, ec);
        std::cout << "Log: " << ec.message() << std::endl;

        cv::Mat image = cv::imread(input_path, cv::IMREAD_COLOR);
        cv::imshow("Original image in color", image);
        cv::waitKey(0);

        if (image.empty())
        {
            std::cerr << "Error: Failed reading image." << std::endl;
            exit(EXIT_FAILURE);
        }
        
        cv::Mat cropped_image;
        Cropper::crop(image, cropped_image);
        cv::imwrite(output_path, cropped_image);
        std::cout << "Log: Saved image at path " << output_path << std::endl;
    }
    else 
    {
        parser.printMessage();
        std::cerr << "Error: Please provide the required arguments." << std::endl;
    }

    return 0;
}