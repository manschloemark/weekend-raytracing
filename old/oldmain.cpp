#include <stdio.h>

int main()
{
	const char* image_name = "testing.ppm";
	const char* file_mode = "w";
	const int image_width = 256;
	const int image_height = 256;

	FILE *image_file = fopen(image_name, file_mode);

	fprintf(image_file, "%s\n", "P3");
	fprintf(image_file, "%d %d\n", image_width, image_height);
	fprintf(image_file, "%d\n", 255);

	for (int j = image_height - 1; j >= 0; --j) {
		fprintf(stdout, "%s %d\n", "Scanlines remaining: ", j);
		for (int i = 0; i < image_width; ++i) {
			auto r = double(i) / (image_width - 1);
			auto g = double(j) / (image_height - 1);
			auto b = 0.25;

			int ir = static_cast<int>(255.999 * r);
			int ig = static_cast<int>(255.999 * g);
			int ib = static_cast<int>(255.999 * b);

			fprintf(image_file, "%d %d %d\n", ir, ig, ib);
		}
	}
	fprintf(stdout, "%s\n", "Done.");
}
