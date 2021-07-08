#include <iostream>
#include <unistd.h>

#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>

#include <argp.h>

#include "common.h"
#include "color.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "moving_sphere.h"
#include "camera.h"
#include "scenes.h"
#include "bvh.h"

// My files
#include "timer.h"
#include "demo_scenes.h"

const char *argp_program_version = "weekend-raytracing 0.2.2";
const char *argp_program_bug_address = "<markofwisdumb@gmail.com>";
static char doc[] = "Weekend Raytracing -- A personal ray-tracer based off of Peter Shirley's _Ray Tracing in One Weekend_ book series.";

static struct argp_option options[] = {
	// Output options
	// TODO :: specify image files of various formats instead of only using PPM and bash redirect
	//{"output", 'o', "FILE", 0, "The file to save the resulting image to. NOTE: should be .ppm format in this version"},
	{"width", 'w', "WIDTH", 0, "Width of output image in pixels.", 0},
	{"height", 'h', "HEIGHT", 0, "Height of output image in pixels.", 0},
	// Render options
	// TODO :: specify scene files insead of hardcoded functions
	{"scene", 's', "SCENE", 0, "Which scene to generate -- SCENE is an integer used in a switch statement.", 1},
	// Performance related
	{"num-samples", 'n', "N_SAMPLES", 0, "Take a sample from each pixel N_SAMPLES times", 2},
	{"max-depth", 'd', "MAX_DEPTH", 0, "MAX_DEPTH is the number of times a ray can be reflected.", 2},
	{"num-threads", 't', "N_THREADS", 0, "Create N_THREADS threads to render the image in parallel. Default is estimated number of cores.", 2},
	// TODO :: should this be a runtime flag or a compile time flag? 
	//         I guess I can try both and see how much it changes the performance. Or not... do I really need the other algorithm?
	// {"moller-trumbore", 'm', 0, 0, "Flag determining which triangle hit algorithm to use -- Moller Trombore or the other one... (what's it called?)", 1},
	// Debugging related
	{"verbose", 'v', 0, 0, "Verbose output. Prints extra info while rendering.", 3},
	// TODO :: implement proper logging capability.
	// {"logfile",         'l', 0, 0, "The file to which log messages will be sent", 2},
	{0} // This needs to be here to argp knows where the options list ends.
};

struct arguments
{
	int scene;
	int image_width, image_height;
	int samples_per_pixel, max_depth, num_threads;
	int verbose;
};

static error_t parse_opt(int key, char *arg, argp_state *state)
{
	arguments *args = (arguments *)state->input;

	// TODO :: some of my options need values -- how do I properly retrieve them? What if they are ints instead of chars? Do I need to parse them?
	switch (key)
	{
	case 's':
		args->scene = atoi(arg);
		break;
	case 'w':
		args->image_width = atoi(arg);
		break;
	case 'h':
		args->image_height = atoi(arg);
		break;
	case 'n':
		args->samples_per_pixel = atoi(arg);
		break;
	case 'd':
		args->max_depth = atoi(arg);
		break;
	case 't':
		args->num_threads = atoi(arg);
		break;
	case 'v':
		args->verbose = 1;
		break;
	/* Removing this for now because I think it should be a compiler flag instead...
	case 'm':
		args->use_mt_alg = atoi(arg);
		break;
	*/
	default:
		return ARGP_ERR_UNKNOWN;
		break;
	}
	return 0;
}

static struct argp argp = {options, parse_opt, 0, doc};

// This struct is used for multithreading
struct pixel_data
{
	color col;
	unsigned int index;
};

color ray_color(const ray& r, const color& background, const hittable& world, int depth)
{
	hit_record rec;

	if (depth <= 0)
	{
		return color(0, 0, 0);
	}

	if (!world.hit(r, 0.001, infinity, rec))
		return background;

	ray scattered;
	color attenuation;

	color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

	if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
		return emitted;

	return emitted + attenuation * ray_color(scattered, background, world, depth - 1);
}


int main(int argc, char *argv[])
{
	nice(1);

	// Set defaults before parsing arguments
	arguments arguments = {0};

	arguments.image_width = 480;
	arguments.image_height = 480;
	arguments.scene = -1;
	arguments.samples_per_pixel = 100;
	arguments.max_depth = 50;
	arguments.num_threads = std::thread::hardware_concurrency() / 2;

	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	// Store values from arguments in primitives so I don't have to refer to arguments all the time
	// (((Is this dumb?)))
	int samples_per_pixel = arguments.samples_per_pixel;
	int max_depth = arguments.max_depth;
	int image_width = arguments.image_width;
	int image_height = arguments.image_height;
	// Default values
	// World / Scene / Camera
	point3 lookfrom(0, 0, 0);
	point3 lookat(0, 0, 1);
	vec3 vup(0, 1, 0);
	auto vfov = 90.0;
	auto aperture = 0.0;
	auto dist_to_focus = (lookat - lookfrom).length();

	hittable_list world;
	color background(0, 0, 0);

	timer t;
	t.start();
	switch(arguments.scene) {
	case 1:
		world = book1_final();
		background = color(0.7, 0.8, 1.0);

		lookfrom = point3(13, 4, 2);
		lookat = point3(0, 0, -2);
		vfov = 40.0;
		break;
	case 2:
		world = book2_final();
		background = color(0, 0, 0);

		lookfrom = point3(478, 278, -600);
		lookat = point3(278, 278, 0);
		vfov = 40.0;
		break;
	case 3:
		world = solar_system();
		background = color(0, 0, 0);
		lookfrom = point3(100, 0, 100);
		lookat = point3(100, 0, 0);
		vfov = 45.0;
		aperture = 0.0;
		dist_to_focus = 100;
		break;
	case 4:

		world = colored_noise_demo();
		background = color(0.5, 0.8, 0.9);

		lookfrom = point3(0, 0, 10);
		lookat = point3(0, 0, 0);
		vfov = 45.0;
		break;
	case 5:
		world = texture_demo();
		background = color(0.7, 0.8, 0.9);

		lookfrom = point3(0, 0, 5);
		lookat = point3(0, 0, -15);
		dist_to_focus = (lookat - lookfrom).length();

		vfov = 30.0;

		break;
	case 6:
		world = rocky_surface_texture_demo();
		background = color(1, 1, 1);
		lookfrom = point3(1920 / 2, 1080 / 2, -5);
		lookat = point3(1920 / 2, 1080 / 2, 0);
		dist_to_focus = 5;
		vfov = 60.0;
		break;
	case 7:
		background = color(0.6, 0.8, 0.9);
		world = random_triangles();
		lookfrom = point3(0, 0, 10);
		lookat = point3(0, 0, 0);
		dist_to_focus = 8.0;
		vfov = 60.0;
		break;
	case 8:
		background = color(0.6, 0.7, 0.9);
		world = triangle_test();
		lookfrom = point3(0, 0, 20);
		lookat = point3(0, 0, -5);
		dist_to_focus = 23.0;
		vfov = 30.0;
		break;
	case 9:
		//background = color(0.2, 0.2, 0.2);
		background = color(0, 0, 0);
		world = glass_and_shadows();
		lookfrom = point3(0, 0, 20);
		lookat = point3(0, 0, -5);
		//dist_to_focus = 10;
		dist_to_focus = 30;
		vfov = 40.0;
		break;
	case 10:
		world = ultimate_demo();
		lookfrom = point3(100, 50, 100);
		lookat = point3(0, 0, 0);
		background = color(0.2, 0.2, 0.2);
		dist_to_focus = (lookat - lookfrom).length();
		vfov = 30.0;
		break;
	default:
		world = cornell_box();
		background = color(0, 0, 0);

		lookfrom = point3(278, 278, -800);
		lookat = point3(278, 278, 0);
		vfov = 40.0;
		break;
	}

	auto aspect_ratio = (double)image_width / (double)image_height;

	camera cam(lookfrom, lookat,
				vup, vfov,
				aspect_ratio,
				aperture, dist_to_focus,
				0.0, 1.0);

	t.stop();
	std::cerr << "It took " << t.duration_ms() << 
				 " milliseconds to load the scene and camera.\n";

	// Create bounding volume hierarchy to speed up collision detection
	// Should I leave this here or should I let scene functions create the bvh?
	t.start();
	bvh_node bvh(world, 0.0, 1.0);
	t.stop();
	std::cerr << "It took " << t.duration_ms() << 
				 " milliseconds to create the bounding volume hierarchy.\n";

	t.start();
	std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";
	color *pixels = (color *)malloc((image_width * image_height) * sizeof(color));

	std::mutex mutex;
	std::condition_variable pixels_cv;

	int num_pixels = image_width * image_height;

#if 0 // Render by lines (Useless with multithreading because I can't actually create enough threads in Linux)
	std::vector<std::future<pixel_data>> pixel_futures;
	std::cerr << "Rendering...\n";
	for(int y = image_height - 1; y >= 0; --y)
	{
		std::cerr << "\r" << y << " lines remaining." << std::flush;
		for(int x = 0; x < image_width; ++x)
		{
		// Make a future for each pixel
			auto future = std::async(std::launch::async,// | std::launch::deferred,
			[&cam, &bvh, &background, &max_depth, &samples_per_pixel,
			x, y, image_width, image_height, &pixels_cv]() -> pixel_data {
						const unsigned int index = (y * image_width) + x;
						color pixel_color(0, 0, 0);
						for(int s = 0; s < samples_per_pixel; ++s)
						{
							float u = float(x + random_double()) / float(image_width - 1);
							float v = float(y + random_double()) / float(image_height - 1);
							ray r = cam.get_ray(u, v);
							pixel_color += ray_color(r, background, bvh, max_depth);
						}
						pixel_data pixel = {};
						pixel.col = normalize(pixel_color, samples_per_pixel);
						pixel.index = index;
						return pixel;
					}
			);

			{
				std::lock_guard<std::mutex> lock(mutex);
				pixel_futures.push_back(std::move(future));
			}
		}
	}

	// Wait until each pixel has been created
	{
		std::unique_lock<std::mutex> lock(mutex);
		pixels_cv.wait(lock, [&pixel_futures, &num_pixels] { 
							return pixel_futures.size() == num_pixels;
		});
	}

	// Get each pixel from the vector of futures and order them
	for(std::future<pixel_data>& pd : pixel_futures)
	{
		pixel_data pixel = pd.get();
		pixels[pixel.index] = pixel.col;
	}
#else // Render by chunks
	// NOTE :: this version has a vector of futures of type vector of pixel_data...
	//         this feels like it will be a bit slower than the original version. But I'll test it.
	//         I have to do this because each chunk is rendering multiple pixels in it's own thread,
	//         which means async() cannot return each pixel individually - it has to return all of the pixels
	//         at once.
	//         Maybe there's a better way to implement multithreading here. Think about it.
	std::vector<std::future<std::vector<pixel_data>>> pixel_futures;

	int thread_count = arguments.num_threads;
	// Determine how many chunks should be along the x and y axis
	// while keeping the chunks as square as possible
	int a = sqrt(thread_count);
	int b;
	while(a != 1)
	{
		b = thread_count / a;
		if(a * b == thread_count)
			break;
		--a;
	}
	int vertical_chunks, horizontal_chunks;
	// I wonder if this will help with caching? IDK
	if(image_height > image_width)
	{
		vertical_chunks = b;
		horizontal_chunks = a;
	}
	else
	{
		vertical_chunks = a;
		horizontal_chunks = b;
	}
	int num_chunks = vertical_chunks * horizontal_chunks;
	int chunk_height = image_height / vertical_chunks;
	int extra_height = image_height % vertical_chunks;
	int chunk_width = image_width / horizontal_chunks;
	int extra_width = image_width % horizontal_chunks;

	if (arguments.verbose != 0)
	{
		std::cerr << "DEBUG CHUNK INFO\n";
		std::cerr << "# THREADS=" << thread_count << "\n";
		std::cerr << "H_CHUNKS=" << horizontal_chunks << " // V_CHUNKS=" << vertical_chunks << " // TOTAL=" << num_chunks << "\n";
		std::cerr << "CHUNK DIMENSIONS=" << chunk_width << "px X " << chunk_height << "px\n";
		std::cerr << "EXTRA HORIZONTAL PIXELS=" << extra_width << " // EXTRA VERTICAL PIXELS=" << extra_height << "\n";
		std::cerr << "PIXELS PER CHUNK=" << chunk_width * chunk_height << "\n";
		std::cerr << "TOTAL PIXELS IN CHUNKS=" << (chunk_width * chunk_height) * num_chunks << "\n";
		std::cerr << "PIXELS IN IMAGE=" << image_height * image_width << "\n";
	}

	int h = chunk_height;
	int w = chunk_width;
	//for(int j = image_height - 1; j >= 0; j = j - h)
	int j = image_height - 1;
	while(j >= 0)
	{
		h = (j < image_height - 1) ? chunk_height : chunk_height + extra_height;
		//for(int i = 0; i < image_width; i = i + w)
		int i = 0;
		while(i < image_width)
		{
			w = (i > 0) ? chunk_width : chunk_width + extra_width;
			// Make a future for each chunk
			auto future = std::async(std::launch::async,// | std::launch::deferred,
			[&cam, &bvh, &background, &max_depth, &samples_per_pixel,
			i, j, w, h, image_width, image_height, &pixels_cv]() -> 
			std::vector<pixel_data> {
						std::vector<pixel_data> chunk_pixels;
						for(int dj = 0; dj < h; ++dj)
						{
							int y = j - dj;
							for (int di = 0; di < w; ++di)
							{
								int x = i + di;
								unsigned int index = (y * image_width) + x;
								color pixel_color(0, 0, 0);
								for (int s = 0; s < samples_per_pixel; ++s)
								{
									auto u = double(x + random_double()) / (image_width - 1);
									auto v = double(y + random_double()) / (image_height - 1);
									ray r = cam.get_ray(u, v);
									pixel_color += ray_color(r, background, bvh, max_depth);
								}
								pixel_data pixel = {};
								pixel.col = normalize(pixel_color, samples_per_pixel);
								pixel.index = index;
								chunk_pixels.push_back(pixel);
							}
						}
						return chunk_pixels;
				}
			);
			{
				std::lock_guard<std::mutex> lock(mutex);
				pixel_futures.push_back(std::move(future));
			}
			i += w;
		}
		j -= h;
	}

	// Wait until each chunk has been created
	{
		std::unique_lock<std::mutex> lock(mutex);
		pixels_cv.wait(lock, [&pixel_futures, &num_chunks] { 
							return pixel_futures.size() >= num_chunks;
		});
	}

	// Get each pixel from the vector of futures and order them
	for(std::future<std::vector<pixel_data>>& ch : pixel_futures)
	{
		std::vector<pixel_data> chunk = ch.get();
		for(pixel_data& pd : chunk)
		{
			pixels[pd.index] = pd.col;
		}
	}
#endif

	std::cerr << "Writing...";
	for(int j = image_height - 1; j >= 0; --j)
		for(int i = 0; i < image_width; ++i)
			write_color(std::cout, (color)(pixels[(j * image_width) + i]), samples_per_pixel);
	free(pixels);
	std::cerr << "\nDONE.\n";
}
