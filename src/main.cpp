#include <iostream>
#include <random>
#include <png++/png.hpp>

void draw_triangle(
	png::image<png::rgb_pixel>& image,
	const uint32_t x0, const uint32_t y0,
	const uint32_t x1, const uint32_t y1,
	const uint32_t x2, const uint32_t y2,
	const uint32_t image_x, const uint32_t image_y,
	const png::rgb_pixel pixel) {
	const auto max_x = std::max(x0, std::max(x1, x2));
	const auto max_y = std::max(y0, std::max(y1, y2));
	const auto min_x = std::min(x0, std::min(x1, x2));
	const auto min_y = std::min(y0, std::min(y1, y2));

#pragma omp parallel for
	for (uint32_t x = min_x; x < max_x; x++) {
		for (uint32_t y = min_y; y < max_y; y++) {
			if (y >= image_y || x >= image_x) break;
			const int32_t dx1 = static_cast<int32_t>(x1) - static_cast<int32_t>(x0);
			const int32_t dy1 = static_cast<int32_t>(y1) - static_cast<int32_t>(y0);
			const int32_t dx2 = static_cast<int32_t>(x2) - static_cast<int32_t>(x0);
			const int32_t dy2 = static_cast<int32_t>(y2) - static_cast<int32_t>(y0);

			const int32_t dxp = static_cast<int32_t>(x) - static_cast<int32_t>(x0);
			const int32_t dyp = static_cast<int32_t>(y) - static_cast<int32_t>(y0);

			const auto det = dx1 * dy2 - dx2 * dy1;
			if (det != 0) {
				const auto a = ( dxp * dy2 - dyp * dx2) / static_cast<double>(det);
				const auto b = (-dxp * dy1 + dyp * dx1) / static_cast<double>(det);

				if (a + b <= 1 && a >= 0 && b >= 0) {
					image.set_pixel(x, y, pixel);
				}
			}
		}
	}
}

void draw_circule(
	png::image<png::rgb_pixel>& image,
	const uint32_t x0, const uint32_t y0,
	const uint32_t r,
	const uint32_t image_x, const uint32_t image_y,
	const png::rgb_pixel pixel) {
	constexpr unsigned scale = 4;

	for (uint32_t i = 0; i < 2 * r; i++) {
		for (uint32_t j = 0; j < 2 * r; j++) {
			double mask = 0;
			for (uint32_t si = 0; si < scale; si++) {
				for (uint32_t sj = 0; sj < scale; sj++) {
					const auto ei = si + i * scale;
					const auto ej = sj + j * scale;

					const auto diff_i = ei - r * scale;
					const auto diff_j = ej - r * scale;

					if (diff_i * diff_i + diff_j * diff_j < (scale * r) * (scale * r)) {
						mask += 1.0;
					}
				}
			}
			mask /= scale * scale;

			const auto ix = static_cast<int32_t>(i) - r + x0;
			const auto iy = static_cast<int32_t>(j) - r + y0;

			if (ix >= 0 && image_x > ix && iy >= 0 && image_y > iy && mask > (0.9 / scale)) {
				const auto back_pixel = image.get_pixel(ix, iy);
				image.set_pixel(ix, iy,
								png::rgb_pixel(
									pixel.red * mask + back_pixel.red * (1 - mask),
									pixel.green * mask + back_pixel.green * (1 - mask),
									pixel.blue * mask + back_pixel.blue * (1 - mask)
									));
			}
		}
	}
}

int main(int argc, char** argv) {
	if (argc < 4) {
		std::fprintf(stderr, "Usage : %s width height output.png [m|s]\n", argv[0]);
		return 1;
	}

	std::mt19937 mt(std::random_device{}());

	const uint32_t output_image_width = std::stoi(argv[1]);
	const uint32_t output_image_height = std::stoi(argv[2]);

	const uint32_t image_width = output_image_width;
	const uint32_t image_height = output_image_height;
	const std::string output_file = argv[3];

	bool star_flag = false;
	bool moon_flag = false;

	if (argc >= 5) {
		const char* additional_flag = argv[4];
		const auto num_flag = std::strlen(additional_flag);
		for (unsigned i = 0; i < num_flag; i++) {
			switch(additional_flag[i]) {
			case 's':
				star_flag = true;
				break;
			case 'm':
				moon_flag = true;
				break;
			default:
				break;
			}
		}
	}

	png::image<png::rgb_pixel> image(image_width, image_height);

	// pallet
	const auto dark_gray_background = png::rgb_pixel(255 * 0.086, 255 * 0.094, 255 * 0.129);
	const auto light_gray_background = png::rgb_pixel(255 * 0.118, 255 * 0.129, 255 * 0.196);
	const auto dark_gray_iceberg = png::rgb_pixel(255 * 0.776, 255 * 0.784, 255 * 0.82);
	std::uniform_int_distribution<int> color_noise_dist(-0, 0);

	// background
#pragma omp parallel for
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
	const auto max_bottom_iceberg_block_width = 4 * iceberg_width / 5;
	const auto min_bottom_iceberg_block_width = iceberg_width / 2;
	const auto max_bottom_iceberg_block_stride = iceberg_width / 6;
	const auto min_bottom_iceberg_block_stride = iceberg_width / 7;
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
