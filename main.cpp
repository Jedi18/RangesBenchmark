#include <range/v3/all.hpp>
#include "hpx/hpx.hpp"
#include "hpx/local/init.hpp"
#include "hpx/algorithm.hpp"
#include "hpx/modules/program_options.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <random>

std::vector<int> original;
std::size_t sum1 = 0, sum2 = 0, sum3 = 0;

int hpx_main(hpx::program_options::variables_map& vm) {
	unsigned int NUM_ITERATIONS = vm["num_iterations"].as<unsigned int>();
	int max_i = vm["max_i"].as<int>();

	std::size_t start = 32;
	std::size_t till = 1 << max_i;

	std::vector<std::array<double, 4>> data;
	original = std::vector<int>(till);
	std::generate(std::begin(original), std::end(original), []() {return std::rand() % 100; });

	for (std::size_t s = start; s <= till; s *= 2)
	{
		std::vector<int> arr(s), res(s);
		std::copy_n(original.begin(), s, arr.begin());

		double seqTime = 0;
		double parTime = 0;
		double rangesTime = 0;

		for (int i = 0; i < NUM_ITERATIONS; i++)
		{
			int sum = 0;
			auto t1 = std::chrono::high_resolution_clock::now();

			hpx::for_each(arr.begin(), arr.end(), [](auto const& elem) {return elem * 2; });
			hpx::transform(arr.begin(), arr.end(), res.begin(), [](auto const& elem) {return elem * 2; });

			auto end1 = std::chrono::high_resolution_clock::now();

			for (int i = 0; i < res.size(); i++) {
				if (rand() % 2 == 0) {
					sum1 += res[i];
				}
			}

			std::chrono::duration<double> time_span1 =
				std::chrono::duration_cast<std::chrono::duration<double>>(
					end1 - t1);

			seqTime += time_span1.count();
		}

		std::copy_n(original.begin(), s, arr.begin());
		res = std::vector<int>(s);

		for (int i = 0; i < NUM_ITERATIONS; i++)
		{
			int sum = 0;
			auto t1 = std::chrono::high_resolution_clock::now();

			hpx::for_each(hpx::execution::par, arr.begin(), arr.end(), [](auto const& elem) {return elem * 2; });
			hpx::transform(hpx::execution::par, arr.begin(), arr.end(), res.begin(), [](auto const& elem) {return elem * 2; });
			
			auto end1 = std::chrono::high_resolution_clock::now();

			for (int i = 0; i < res.size(); i++) {
				if (rand() % 2 == 0) {
					sum2 += res[i];
				}
			}

			std::chrono::duration<double> time_span1 =
				std::chrono::duration_cast<std::chrono::duration<double>>(
					end1 - t1);

			parTime += time_span1.count();
		}

		std::copy_n(original.begin(), s, arr.begin());
		res = std::vector<int>(s);

		for (int i = 0; i < NUM_ITERATIONS; i++)
		{
			int sum = 0;
			auto t2 = std::chrono::high_resolution_clock::now();
			std::chrono::time_point<std::chrono::high_resolution_clock> end2;

			auto rng2 = arr | ranges::views::transform([](auto const& elem) {return elem * 2; });
			hpx::ranges::transform(hpx::execution::par, rng2, res.begin(), [](auto const& elem) {return elem * 2; });

			end2 = std::chrono::high_resolution_clock::now();

			for (int i = 0; i < res.size(); i++) {
				if (rand() % 2 == 0) {
					sum2 += res[i];
				}
			}

			std::chrono::duration<double> time_span2 =
				std::chrono::duration_cast<std::chrono::duration<double>>(
					end2 - t2);

			rangesTime += time_span2.count();
		}

		seqTime /= NUM_ITERATIONS;
		parTime /= NUM_ITERATIONS;
		rangesTime /= NUM_ITERATIONS;

		data.push_back(std::array<double, 4>{(double)s, seqTime, parTime, rangesTime});
		std::cout << "N : " << s << '\n';
		std::cout << "sum1 : " << sum1 << '\n';
		std::cout << "sum2 : " << sum2 << '\n';
		std::cout << "sum3 : " << sum3 << '\n';
		std::cout << "SEQ: " << seqTime << '\n';
		std::cout << "PAR: " << parTime << '\n';
		std::cout << "RANGES: " << rangesTime << "\n\n";

		sum1 = 0;
		sum2 = 0;
		sum3 = 0;
	}

	std::ofstream outputFile;
	outputFile = std::ofstream("for_each_transform.csv");

	for (auto& d : data)
	{
		outputFile << d[0] << "," << d[1] << "," << d[2] << "," << d[3] << ","
			<< ",\n";
	}

	return hpx::local::finalize();
}

int main(int argc, char* argv[])
{
	using namespace hpx::program_options;
	options_description desc_commandline("usage" HPX_APPLICATION_STRING " [options]");

	desc_commandline.add_options()("num_iterations", hpx::program_options::value<unsigned int>()->default_value(5), "number of iterations (default: 5)")
		("max_i", hpx::program_options::value<int>()->default_value(28), "max size 2^i (default: 28)");

	// Initialize and run HPX
	hpx::local::init_params init_args;
	init_args.desc_cmdline = desc_commandline;

	return hpx::local::init(hpx_main, argc, argv, init_args);
}
