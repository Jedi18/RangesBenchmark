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

enum class ALGORITHM_NAME
{
	UNIQUE_REPLACE_IF_FOR,
	UNIQUE_REMOVE_IF_FOR,
	REVERSE_REPLACE_IF_FOR,
	REVERSE_REMOVE_IF_FOR,
	FOR_EACH_TRANSFORM,
	REPLACE_IF_FOR,
	REMOVE_IF_FOR,
	UNIQUE_FOR,
	REVERSE_FOR,
};

int hpx_main(hpx::program_options::variables_map& vm) {
	std::string algorithm_name = vm["algorithm_name"].as<std::string>();
	unsigned int NUM_ITERATIONS = vm["num_iterations"].as<unsigned int>();
	int max_i = vm["max_i"].as<int>();

	std::map<std::string, ALGORITHM_NAME> nameToAlgo = {
		{"UNIQUE_REPLACE_IF_FOR", ALGORITHM_NAME::UNIQUE_REPLACE_IF_FOR },
		{"UNIQUE_REMOVE_IF_FOR", ALGORITHM_NAME::UNIQUE_REMOVE_IF_FOR },
		{"REVERSE_REPLACE_IF_FOR", ALGORITHM_NAME::REVERSE_REPLACE_IF_FOR },
		{"REVERSE_REMOVE_IF_FOR", ALGORITHM_NAME::REVERSE_REMOVE_IF_FOR },
		{"FOR_EACH_TRANSFORM", ALGORITHM_NAME::FOR_EACH_TRANSFORM },
		{"REPLACE_IF_FOR", ALGORITHM_NAME::REPLACE_IF_FOR },
		{"REMOVE_IF_FOR", ALGORITHM_NAME::REMOVE_IF_FOR },
		{"UNIQUE_FOR", ALGORITHM_NAME::UNIQUE_FOR },
		{"REVERSE_FOR", ALGORITHM_NAME::REVERSE_FOR }
	};

	ALGORITHM_NAME algo = nameToAlgo[algorithm_name];
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

			switch (algo)
			{
			case ALGORITHM_NAME::UNIQUE_REPLACE_IF_FOR:
			{
				auto iter = hpx::unique(arr.begin(), arr.end());
				hpx::replace_if(arr.begin(), iter, [](auto const& elem) {return elem == 2; }, 3);
				hpx::for_each(arr.begin(), iter, [](auto const& elem) { return elem * 2; });
				break;
			}
			case ALGORITHM_NAME::UNIQUE_REMOVE_IF_FOR:
			{
				auto iter = hpx::unique(arr.begin(), arr.end());
				hpx::remove_if(arr.begin(), iter, [](auto const& elem) {return elem == 2; });
				hpx::for_each(arr.begin(), iter, [](auto const& elem) { return elem * 2; });
				break;
			}
			case ALGORITHM_NAME::REVERSE_REPLACE_IF_FOR:
			{
				hpx::reverse(arr.begin(), arr.end());
				hpx::replace_if(arr.begin(), arr.end(), [](auto const& elem) {return elem == 2; }, 3);
				hpx::for_each(arr.begin(), arr.end(), [](auto const& elem) { return elem * 2; });
				break;
			}
			case ALGORITHM_NAME::REVERSE_REMOVE_IF_FOR:
			{
				hpx::reverse(arr.begin(), arr.end());
				hpx::remove_if(arr.begin(), arr.end(), [](auto const& elem) {return elem == 2; });
				hpx::for_each(arr.begin(), arr.end(), [](auto const& elem) { return elem * 2; });
				break;
			}
			case ALGORITHM_NAME::FOR_EACH_TRANSFORM:
			{
				hpx::for_each(arr.begin(), arr.end(), [](auto const& elem) {return elem * 2; });
				hpx::transform(arr.begin(), arr.end(), res.begin(), [](auto const& elem) {return elem * 2; });
				break;
			}
			case ALGORITHM_NAME::REPLACE_IF_FOR:
			{
				hpx::replace_if(arr.begin(), arr.end(), [](auto const& elem) {return elem == 2; }, 3);
				hpx::for_each(arr.begin(), arr.end(), [](auto const& elem) { return elem * 2; });
				break;
			}
			case ALGORITHM_NAME::REMOVE_IF_FOR:
			{
				hpx::remove_if(arr.begin(), arr.end(), [](auto const& elem) {return elem == 2; });
				hpx::for_each(arr.begin(), arr.end(), [](auto const& elem) { return elem * 2; });
				break;
			}
			case ALGORITHM_NAME::UNIQUE_FOR:
			{
				auto iter = hpx::unique(arr.begin(), arr.end());
				hpx::for_each(arr.begin(), iter, [](auto const& elem) { return elem * 2; });
				break;
			}
			case ALGORITHM_NAME::REVERSE_FOR:
			{
				hpx::reverse(arr.begin(), arr.end());
				hpx::for_each(arr.begin(), arr.end(), [](auto const& elem) { return elem * 2; });
				break;
			}
			}

			auto end1 = std::chrono::high_resolution_clock::now();

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

			switch (algo)
			{
			case ALGORITHM_NAME::UNIQUE_REPLACE_IF_FOR:
			{
				auto iter = hpx::unique(hpx::execution::par, arr.begin(), arr.end());
				hpx::replace_if(hpx::execution::par, arr.begin(), iter, [](auto const& elem) {return elem == 2; }, 3);
				hpx::for_each(hpx::execution::par, arr.begin(), iter, [](auto const& elem) { return elem * 2; });
				break;
			}
			case ALGORITHM_NAME::UNIQUE_REMOVE_IF_FOR:
			{
				auto iter = hpx::unique(hpx::execution::par, arr.begin(), arr.end());
				hpx::remove_if(hpx::execution::par, arr.begin(), iter, [](auto const& elem) {return elem == 2; });
				hpx::for_each(hpx::execution::par, arr.begin(), iter, [](auto const& elem) { return elem * 2; });
				break;
			}
			case ALGORITHM_NAME::REVERSE_REPLACE_IF_FOR:
			{
				hpx::reverse(hpx::execution::par, arr.begin(), arr.end());
				hpx::replace_if(hpx::execution::par, arr.begin(), arr.end(), [](auto const& elem) {return elem == 2; }, 3);
				hpx::for_each(hpx::execution::par, arr.begin(), arr.end(), [](auto const& elem) { return elem * 2; });
				break;
			}
			case ALGORITHM_NAME::REVERSE_REMOVE_IF_FOR:
			{
				hpx::reverse(hpx::execution::par, arr.begin(), arr.end());
				hpx::remove_if(hpx::execution::par, arr.begin(), arr.end(), [](auto const& elem) {return elem == 2; });
				hpx::for_each(hpx::execution::par, arr.begin(), arr.end(), [](auto const& elem) { return elem * 2; });
				break;
			}
			case ALGORITHM_NAME::FOR_EACH_TRANSFORM:
			{
				hpx::for_each(hpx::execution::par, arr.begin(), arr.end(), [](auto const& elem) {return elem * 2; });
				hpx::transform(hpx::execution::par, arr.begin(), arr.end(), res.begin(), [](auto const& elem) {return elem * 2; });
				break;
			}
			case ALGORITHM_NAME::REPLACE_IF_FOR:
			{
				hpx::replace_if(hpx::execution::par, arr.begin(), arr.end(), [](auto const& elem) {return elem == 2; }, 3);
				hpx::for_each(hpx::execution::par, arr.begin(), arr.end(), [](auto const& elem) { return elem * 2; });
				break;
			}
			case ALGORITHM_NAME::REMOVE_IF_FOR:
			{
				hpx::remove_if(hpx::execution::par, arr.begin(), arr.end(), [](auto const& elem) {return elem == 2; });
				hpx::for_each(hpx::execution::par, arr.begin(), arr.end(), [](auto const& elem) { return elem * 2; });
				break;
			}
			case ALGORITHM_NAME::UNIQUE_FOR:
			{
				auto iter = hpx::unique(hpx::execution::par, arr.begin(), arr.end());
				hpx::for_each(hpx::execution::par, arr.begin(), iter, [](auto const& elem) { return elem * 2; });
				break;
			}
			case ALGORITHM_NAME::REVERSE_FOR:
			{
				hpx::reverse(hpx::execution::par, arr.begin(), arr.end());
				hpx::for_each(hpx::execution::par, arr.begin(), arr.end(), [](auto const& elem) { return elem * 2; });
				break;
			}
			}
			auto end1 = std::chrono::high_resolution_clock::now();

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

			switch (algo)
			{
			case ALGORITHM_NAME::UNIQUE_REPLACE_IF_FOR:
			{
				auto rng1 = ranges::views::unique(arr);
				auto rng2 = rng1 | ranges::views::replace_if([](auto const& elem) {return elem == 2; }, 3);
				hpx::ranges::for_each(hpx::execution::par, rng2, [](auto const& elem) { return elem * 2; });
				break;
			}
			case ALGORITHM_NAME::UNIQUE_REMOVE_IF_FOR:
			{
				auto rng1 = ranges::views::unique(arr);
				auto rng2 = rng1 | ranges::views::remove_if([](auto const& elem) {return elem == 2; });
				hpx::ranges::for_each(hpx::execution::par, rng2, [](auto const& elem) { return elem * 2; });
				break;
			}
			case ALGORITHM_NAME::REVERSE_REPLACE_IF_FOR:
			{
				auto rng1 = arr | ranges::views::reverse;
				auto rng2 = rng1 | ranges::views::replace_if([](auto const& elem) {return elem == 2; }, 3);
				hpx::ranges::for_each(hpx::execution::par, rng2, [](auto const& elem) { return elem * 2; });
				break;
			}
			case ALGORITHM_NAME::REVERSE_REMOVE_IF_FOR:
			{
				auto rng1 = arr | ranges::views::reverse;
				auto rng2 = rng1 | ranges::views::remove_if([](auto const& elem) {return elem == 2; });
				hpx::ranges::for_each(hpx::execution::par, rng2, [](auto const& elem) { return elem * 2; });
				break;
			}
			case ALGORITHM_NAME::FOR_EACH_TRANSFORM:
			{
				auto rng2 = arr | ranges::views::transform([](auto const& elem) {return elem * 2; });
				hpx::ranges::transform(hpx::execution::par, rng2, res.begin(), [](auto const& elem) {return elem * 2; });
				break;
			}
			case ALGORITHM_NAME::REPLACE_IF_FOR:
			{
				auto rng2 = arr | ranges::views::replace_if([](auto const& elem) {return elem == 2; }, 3);
				hpx::ranges::for_each(hpx::execution::par, rng2, [](auto const& elem) { return elem * 2; });
				break;
			}
			case ALGORITHM_NAME::REMOVE_IF_FOR:
			{
				auto rng2 = arr | ranges::views::remove_if([](auto const& elem) {return elem == 2; });
				hpx::ranges::for_each(hpx::execution::par, rng2, [](auto const& elem) { return elem * 2; });
				break;
			}
			case ALGORITHM_NAME::UNIQUE_FOR:
			{
				auto rng1 = ranges::views::unique(arr);
				hpx::ranges::for_each(hpx::execution::par, rng1, [](auto const& elem) { return elem * 2; });
				break;
			}
			case ALGORITHM_NAME::REVERSE_FOR:
			{
				auto rng1 = arr | ranges::views::reverse;
				hpx::ranges::for_each(hpx::execution::par, rng1, [](auto const& elem) { return elem * 2; });
				break;
			}
			}
			auto end2 = std::chrono::high_resolution_clock::now();

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
		std::cout << "SEQ: " << seqTime << '\n';
		std::cout << "PAR: " << parTime << '\n';
		std::cout << "RANGES: " << rangesTime << "\n\n";
	}

	std::ofstream outputFile;
	switch (algo)
	{
	case ALGORITHM_NAME::UNIQUE_REPLACE_IF_FOR:
	{
		outputFile = std::ofstream("unique_replace_if_for_each.csv");
		break;
	}
	case ALGORITHM_NAME::UNIQUE_REMOVE_IF_FOR:
	{
		outputFile = std::ofstream("unique_remove_if_for_each.csv");
		break;
	}
	case ALGORITHM_NAME::REVERSE_REPLACE_IF_FOR:
	{
		outputFile = std::ofstream("reverse_replace_if_for_each.csv");
		break;
	}
	case ALGORITHM_NAME::REVERSE_REMOVE_IF_FOR:
	{
		outputFile = std::ofstream("reverse_remove_if_for_each.csv");
		break;
	}
	case ALGORITHM_NAME::FOR_EACH_TRANSFORM:
	{
		outputFile = std::ofstream("for_each_transform.csv");
		break;
	}
	case ALGORITHM_NAME::REPLACE_IF_FOR:
	{
		outputFile = std::ofstream("replace_if_for_each.csv");
		break;
	}
	case ALGORITHM_NAME::REMOVE_IF_FOR:
	{
		outputFile = std::ofstream("remove_if_for_each.csv");
		break;
	}
	case ALGORITHM_NAME::UNIQUE_FOR:
	{
		outputFile = std::ofstream("unique_for_each.csv");
		break;
	}
	case ALGORITHM_NAME::REVERSE_FOR:
	{
		outputFile = std::ofstream("reverse_for_each.csv");
		break;
	}
	default:
	{
		outputFile = std::ofstream("rangeAlgo.csv");
	}
	}

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

	desc_commandline.add_options()("algorithm_name", hpx::program_options::value<std::string>()->default_value("UNIQUE_REPLACE_IF_FOR"), "algorithm name (default: UNIQUE_REPLACE_IF_FOR)")
		("num_iterations", hpx::program_options::value<unsigned int>()->default_value(5), "number of iterations (default: 5)")
		("max_i", hpx::program_options::value<int>()->default_value(28), "max size 2^i (default: 28)");

	// Initialize and run HPX
	hpx::local::init_params init_args;
	init_args.desc_cmdline = desc_commandline;

	return hpx::local::init(hpx_main, argc, argv, init_args);
}
