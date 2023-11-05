#include "cropper.hpp"

void Cropper::crop(const cv::Mat &src_image, cv::Mat &dst_image)
{
    // Morphological operations before blurring
    // (good for object segmentation, boundary extraction, filling gaps)
    cv::Mat grayscaled_image;
    cv::Mat dilated_image;
    cv::Mat eroded_image;
    cv::Mat blurred_image;
    cv::Mat binary_image;

    cv::cvtColor(src_image, grayscaled_image, cv::COLOR_BGR2GRAY);

    Cropper::dilate(grayscaled_image, dilated_image);
    Cropper::erode(dilated_image, eroded_image);
    cv::GaussianBlur(eroded_image, blurred_image, cv::Size(3, 3), 9);
    cv::Canny(blurred_image, binary_image, 0, 100);

    std::vector<cv::Vec4i> hierarchy;
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(binary_image, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    cv::Mat image_with_contours = src_image.clone();
    cv::drawContours(image_with_contours, contours, -1, cv::Scalar(0, 0, 255), 2);
    cv::imshow("Image with original contours", image_with_contours);
    cv::waitKey(0);

    std::vector<std::vector<cv::Point>> rectangle_contours;
    std::vector<std::vector<cv::Point>> simplified_contours;

    for (const auto &contour : contours)
    {
        double perimeter = cv::arcLength(contour, true);
        double epsilon = 0.1 * perimeter;

        std::vector<cv::Point> simplified_contour;
        cv::approxPolyDP(contour, simplified_contour, epsilon, true);
        simplified_contours.push_back(simplified_contour);
    }

    // Angle must be close to 90 degrees
    for (const auto &contour : simplified_contours)
    {
        if (contour.size() < 3)
            continue;

        int right_angles = 0;
        for (size_t i = 0; i < contour.size() - 2; ++i)
        {
            const cv::Point &A = contour[i];
            const cv::Point &B = contour[i + 1];
            const cv::Point &C = contour[i + 2];
            // std::cout << "Point A: (" << A.x << ", " << A.y << ")" << std::endl;
            // std::cout << "Point B: (" << B.x << ", " << B.y << ")" << std::endl;
            // std::cout << "Point C: (" << C.x << ", " << C.y << ")" << std::endl;
            if (is_angle_right(A, B, C))
                ++right_angles;
        }

        std::cout << "Log: Right angles number: " << right_angles << std::endl;

        // Because of CHAIN_APPROX_SIMPLE
        if (right_angles >= 2)
            rectangle_contours.push_back(contour);
    }

    // std::cout << "Log: Contours with right angles number: " << rectangle_contours.size() << std::endl;

    // image_with_contours = src_image.clone();
    // cv::drawContours(image_with_contours, rectangle_contours, -1, cv::Scalar(0, 0, 255), 2);

    // cv::imshow("Image with Rectangle Contours", image_with_contours);

    // Now select the largest contour that is not more than 85% of the image size
    // Order by largest to smallest

    sort(rectangle_contours.begin(), rectangle_contours.end(),
         [](const std::vector<cv::Point> &contour_1, const std::vector<cv::Point> &contour_2)
         {
             return cv::contourArea(contour_1, false) > cv::contourArea(contour_2, false);
         });

    std::vector<cv::Point> image_contour = Cropper::get_contour(src_image);
    double image_contour_area = cv::contourArea(image_contour);

    for (const auto &contour : rectangle_contours)
    {
        // It must be no larger than 85% of the image
        if (image_contour_area * 0.85 >= cv::contourArea(contour))
        {
            std::cout << "Log: Found potential frame!" << std::endl;
            cv::Rect bounding_rect = cv::boundingRect(contour);
            cv::Mat image_with_rectangle = src_image.clone();
            cv::rectangle(image_with_rectangle, bounding_rect.tl(), bounding_rect.br(), cv::Scalar(0, 0, 255), 2);
            cv::imshow("Image with rectangle", image_with_rectangle);
            cv::waitKey(0);

            dst_image = src_image(bounding_rect);
            cv::imshow("Cropped image", dst_image);
            cv::waitKey(0);
            break;
        }
    }
}

bool Cropper::is_angle_right(const cv::Point &A, const cv::Point &B, const cv::Point &C, double tolerance)
{
    cv::Point AB = B - A;
    cv::Point BC = C - B;

    double dot_product = AB.x * BC.x + AB.y * BC.y;

    double magnitude_AB = std::sqrt(AB.x * AB.x + AB.y * AB.y);
    double magnitude_BC = std::sqrt(BC.x * BC.x + BC.y * BC.y);

    double cosine_angle = dot_product / (magnitude_AB * magnitude_BC);
    double angle = std::acos(cosine_angle) * 180.0 / CV_PI;

    return std::abs(90.0 - angle) <= tolerance;
}

void Cropper::erode(const cv::Mat &src_image, cv::Mat &dst_image, int intensity)
{
    if (src_image.empty())
    {
        std::cerr << "Error: Image is empty!" << std::endl;
        exit(EXIT_FAILURE);
    }
    cv::Mat structuring_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * intensity + 1, 2 * intensity + 1), cv::Point(intensity, intensity));
    cv::erode(src_image, dst_image, structuring_element);
}

void Cropper::dilate(const cv::Mat &src_image, cv::Mat &dst_image, int intensity)
{
    if (src_image.empty())
    {
        std::cerr << "Error: Image is empty!" << std::endl;
        exit(EXIT_FAILURE);
    }
    cv::Mat structuring_element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * intensity + 1, 2 * intensity + 1), cv::Point(intensity, intensity));
    cv::dilate(src_image, dst_image, structuring_element);
}

std::vector<cv::Point> Cropper::get_contour(const cv::Mat &image)
{
    std::vector<cv::Point> contour;

    if (image.empty())
    {
        std::cerr << "Error: Image is empty!" << std::endl;
        exit(EXIT_FAILURE);
    }

    contour.push_back(cv::Point(0, 0));
    contour.push_back(cv::Point(image.cols, 0));
    contour.push_back(cv::Point(image.cols, image.rows));
    contour.push_back(cv::Point(0, image.rows));

    return contour;
}
