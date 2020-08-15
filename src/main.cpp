#include <iostream>
#include <random>
#include <png++/png.hpp>

int main(int argc, char** argv) {
	if (argc < 4) {
		std::fprintf(stderr, "Usage : %s width height output.png\n", argv[0]);
		return 1;
	}

	std::mt19937 mt(std::random_device{}());

	const uint32_t image_width = std::stoi(argv[1]);
	const uint32_t image_height = std::stoi(argv[2]);
	const std::string output_file = argv[3];

	png::image<png::rgb_pixel> image(image_width, image_height);

	// pallet
	const auto dark_gray_background = png::rgb_pixel(255 * 0.086, 255 * 0.094, 255 * 0.129);
	const auto light_gray_background = png::rgb_pixel(255 * 0.118, 255 * 0.129, 255 * 0.196);
	const auto dark_gray_iceberg = png::rgb_pixel(255 * 0.776, 255 * 0.784, 255 * 0.82);
	std::uniform_int_distribution<int> color_noise_dist(-2, 2);

	// background
	for (uint32_t w = 0; w < image_width; w++) {
		for (uint32_t h = 0; h < image_height / 2; h++) {
			image[h][w] = dark_gray_background;
		}
		for (uint32_t h = image_height / 2; h < image_height; h++) {
			image[h][w] = light_gray_background;
		}
	}

	// iceberg
	const auto end_w = 2 * image_width / 3;
	const auto iceberg_width = image_width / 3;

	auto top_start_w = image_width / 3;
	const auto top_iceberg_y = image_height / 2;
	const auto max_top_iceberg_block_width = iceberg_width / 2;
	const auto min_top_iceberg_block_width = iceberg_width / 3;
	const auto max_top_iceberg_block_stride = iceberg_width / 4;
	const auto min_top_iceberg_block_stride = iceberg_width / 5;
	std::uniform_int_distribution<uint32_t> dist_top_iceberg_block_width(min_top_iceberg_block_width, max_top_iceberg_block_width);
	std::uniform_int_distribution<uint32_t> dist_top_iceberg_block_stride(min_top_iceberg_block_stride, max_top_iceberg_block_stride);

	auto bottom_start_w = image_width / 3;
	const auto bottom_iceberg_y = image_height / 2;
	const auto max_bottom_iceberg_block_width = iceberg_width;
	const auto min_bottom_iceberg_block_width = iceberg_width / 2;
	const auto max_bottom_iceberg_block_stride = iceberg_width / 3;
	const auto min_bottom_iceberg_block_stride = iceberg_width / 4;
	std::uniform_int_distribution<uint32_t> dist_bottom_iceberg_block_width(min_bottom_iceberg_block_width, max_bottom_iceberg_block_width);
	std::uniform_int_distribution<uint32_t> dist_bottom_iceberg_block_stride(min_bottom_iceberg_block_stride, max_bottom_iceberg_block_stride);

	// top
	while (1) {
		auto top_iceberg_block_width = dist_top_iceberg_block_width(mt);

		if (top_start_w + top_iceberg_block_width > end_w) {
			top_iceberg_block_width -= (top_start_w + top_iceberg_block_width - end_w);
		}

		const auto light_gray_iceberg = png::rgb_pixel(
			255 * 0.776 + color_noise_dist(mt),
			255 * 0.784 + color_noise_dist(mt),
			255 * 0.820 + color_noise_dist(mt));
		const auto dark_gray_iceberg = png::rgb_pixel(
			255 * 0.42 + color_noise_dist(mt),
			255 * 0.439 + color_noise_dist(mt),
			255 * 0.53 + color_noise_dist(mt));

		draw_triangle(
			image,
			top_start_w, top_iceberg_y,
			top_start_w + top_iceberg_block_width, top_iceberg_y,
			top_start_w + top_iceberg_block_width / 2, top_iceberg_y - top_iceberg_block_width / 2,
			image_width, image_height,
			light_gray_iceberg
			);

		const auto stride = dist_top_iceberg_block_stride(mt);
		top_start_w += stride;

		if (top_start_w > end_w) {
			break;
		}
	}

	// bottom
	while (1) {
		auto bottom_iceberg_block_width = dist_bottom_iceberg_block_width(mt);

		if (bottom_start_w + bottom_iceberg_block_width > end_w) {
			bottom_iceberg_block_width -= (bottom_start_w + bottom_iceberg_block_width - end_w);
		}

		const auto dark_gray_iceberg = png::rgb_pixel(
			255 * 0.42 + color_noise_dist(mt),
			255 * 0.439 + color_noise_dist(mt),
			255 * 0.53 + color_noise_dist(mt));

		draw_triangle(
			image,
			bottom_start_w, bottom_iceberg_y,
			bottom_start_w + bottom_iceberg_block_width, bottom_iceberg_y,
			bottom_start_w + bottom_iceberg_block_width / 2, bottom_iceberg_y + bottom_iceberg_block_width / 2,
			image_width, image_height,
			dark_gray_iceberg
			);

		const auto stride = dist_bottom_iceberg_block_stride(mt);
		bottom_start_w += stride;

		if (bottom_start_w > end_w) {
			break;
		}
	}

	image.write(output_file);
}
