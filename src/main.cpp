#include <iostream>
#include <png++/png.hpp>

int main(int argc, char** argv) {
	if (argc < 4) {
		std::fprintf(stderr, "Usage : %s width height output.png\n", argv[0]);
		return 1;
	}

	const uint32_t image_width = std::stoi(argv[1]);
	const uint32_t image_height = std::stoi(argv[2]);
	const std::string output_file = argv[3];

	png::image<png::rgb_pixel> image(image_width, image_height);

	// pallet
	const auto dark_gray = png::rgb_pixel(255 * 0.086, 255 * 0.094, 255 * 0.129);
	const auto light_gray = png::rgb_pixel(255 * 0.118, 255 * 0.129, 255 * 0.196);

	for (uint32_t w = 0; w < image_width; w++) {
		for (uint32_t h = 0; h < image_height / 2; h++) {
			image[h][w] = dark_gray;
		}
		for (uint32_t h = image_height / 2; h < image_height; h++) {
			image[h][w] = light_gray;
		}
	}

	image.write(output_file);
}
