#include <range/v3/all.hpp>
#include "hpx/hpx.hpp"
#include "hpx/hpx_main.hpp"
#include "hpx/algorithm.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <map>
#include <string>

//#define UNIQUE_REPLACE_IF_FOR
//#define UNIQUE_REMOVE_IF_FOR
//#define REVERSE_REPLACE_IF_FOR
//#define REVERSE_REMOVE_IF_FOR
#define FOR_EACH_TRANSFORM

int main()
{
	std::size_t start = 32;
	std::size_t till = 1 << 28;

	const auto NUM_ITERATIONS = 50;

	std::vector<std::array<double, 4>> data;

	for (std::size_t s = start; s <= till; s *= 2)
	{
		std::vector<int> arr(s), res(s);
		std::iota(std::begin(arr), std::end(arr), 1);

		double seqTime = 0;
		double parTime = 0;
		double rangesTime = 0;

		for (int i = 0; i < NUM_ITERATIONS + 5; i++)
		{
			std::vector<int> res(s);
			int sum = 0;
			auto t1 = std::chrono::high_resolution_clock::now();
#if defined(UNIQUE_REPLACE_IF_FOR)
			auto iter = hpx::unique(arr.begin(), arr.end());
			hpx::replace_if(arr.begin(), iter, [](auto const& elem) {return elem == 2; }, 3);
			hpx::for_each(arr.begin(), iter, [](auto const& elem) { return elem * 2; });
#elif defined(UNIQUE_REMOVE_IF_FOR)
			auto iter = hpx::unique(arr.begin(), arr.end());
			hpx::remove_if(arr.begin(), iter, [](auto const& elem) {return elem == 2; });
			hpx::for_each(arr.begin(), iter, [](auto const& elem) { return elem * 2; });
#elif defined(REVERSE_REPLACE_IF_FOR)
			hpx::reverse(arr.begin(), arr.end());
			hpx::replace_if(arr.begin(), arr.end(), [](auto const& elem) {return elem == 2; }, 3);
			hpx::for_each(arr.begin(), arr.end(), [](auto const& elem) { return elem * 2; });
#elif defined(REVERSE_REMOVE_IF_FOR)
			hpx::reverse(arr.begin(), arr.end());
			hpx::remove_if(arr.begin(), arr.end(), [](auto const& elem) {return elem == 2; });
			hpx::for_each(arr.begin(), arr.end(), [](auto const& elem) { return elem * 2; });
#elif defined(FOR_EACH_TRANSFORM)
			hpx::for_each(arr.begin(), arr.end(), [](auto const& elem) {return elem * 2; });
			hpx::transform(arr.begin(), arr.end(), res.begin(), [](auto const& elem) {return elem * 2; });
#endif
			for (auto elem : arr) {
				sum += elem;
			}
			auto end1 = std::chrono::high_resolution_clock::now();

			// don't consider first 5 iterations
			if (NUM_ITERATIONS < 5)
			{
				continue;
			}

			std::chrono::duration<double> time_span1 =
				std::chrono::duration_cast<std::chrono::duration<double>>(
					end1 - t1);

			seqTime += time_span1.count();
		}

		for (int i = 0; i < NUM_ITERATIONS + 5; i++)
		{
			std::vector<int> res(s);
			int sum = 0;
			auto t1 = std::chrono::high_resolution_clock::now();
#if defined(UNIQUE_REPLACE_IF_FOR)
			auto iter = hpx::unique(hpx::execution::par, arr.begin(), arr.end());
			hpx::replace_if(hpx::execution::par, arr.begin(), iter, [](auto const& elem) {return elem == 2; }, 3);
			hpx::for_each(hpx::execution::par, arr.begin(), iter, [](auto const& elem) { return elem * 2; });
#elif defined(UNIQUE_REMOVE_IF_FOR)
			auto iter = hpx::unique(hpx::execution::par, arr.begin(), arr.end());
			hpx::remove_if(hpx::execution::par, arr.begin(), iter, [](auto const& elem) {return elem == 2; });
			hpx::for_each(hpx::execution::par, arr.begin(), iter, [](auto const& elem) { return elem * 2; });
#elif defined(REVERSE_REPLACE_IF_FOR)
			hpx::reverse(hpx::execution::par, arr.begin(), arr.end());
			hpx::replace_if(hpx::execution::par, arr.begin(), arr.end(), [](auto const& elem) {return elem == 2; }, 3);
			hpx::for_each(hpx::execution::par, arr.begin(), arr.end(), [](auto const& elem) { return elem * 2; });
#elif defined(REVERSE_REMOVE_IF_FOR)
			hpx::reverse(hpx::execution::par, arr.begin(), arr.end());
			hpx::remove_if(hpx::execution::par, arr.begin(), arr.end(), [](auto const& elem) {return elem == 2; });
			hpx::for_each(hpx::execution::par, arr.begin(), arr.end(), [](auto const& elem) { return elem * 2; });
#elif defined(FOR_EACH_TRANSFORM)
			hpx::for_each(hpx::execution::par, arr.begin(), arr.end(), [](auto const& elem) {return elem * 2; });
			hpx::transform(hpx::execution::par, arr.begin(), arr.end(), res.begin(), [](auto const& elem) {return elem * 2; });
#endif
			for (auto elem : arr) {
				sum += elem;
			}
			auto end1 = std::chrono::high_resolution_clock::now();

			// don't consider first 5 iterations
			if (NUM_ITERATIONS < 5)
			{
				continue;
			}

			std::chrono::duration<double> time_span1 =
				std::chrono::duration_cast<std::chrono::duration<double>>(
					end1 - t1);

			parTime += time_span1.count();
		}

		for (int i = 0; i < NUM_ITERATIONS + 5; i++)
		{
			std::vector<int> res1(s);
			int sum = 0;
			auto t2 = std::chrono::high_resolution_clock::now();
#if defined(UNIQUE_REPLACE_IF_FOR)
			auto rng1 = ranges::views::unique(arr);
			auto rng2 = rng1 | ranges::views::replace_if([](auto const& elem) {return elem == 2; }, 3);
			hpx::ranges::for_each(hpx::execution::par, rng2, [](auto const& elem) { return elem * 2; });
#elif defined(UNIQUE_REMOVE_IF_FOR)
			auto rng1 = ranges::views::unique(arr);
			auto rng2 = rng1 | ranges::views::remove_if([](auto const& elem) {return elem == 2; });
			hpx::ranges::for_each(hpx::execution::par, rng2, [](auto const& elem) { return elem * 2; });
#elif defined(REVERSE_REPLACE_IF_FOR)
			auto rng1 = arr | ranges::views::reverse;
			auto rng2 = rng1 | ranges::views::replace_if([](auto const& elem) {return elem == 2; }, 3);
			hpx::ranges::for_each(hpx::execution::par, rng2, [](auto const& elem) { return elem * 2; });
#elif defined(REVERSE_REMOVE_IF_FOR)
			auto rng1 = arr | ranges::views::reverse;
			auto rng2 = rng1 | ranges::views::remove_if([](auto const& elem) {return elem == 2; });
			hpx::ranges::for_each(hpx::execution::par, rng2, [](auto const& elem) { return elem * 2; });
#elif defined(FOR_EACH_TRANSFORM)
			auto rng2 = arr | ranges::views::transform([](auto const& elem) {return elem * 2; });
			hpx::ranges::transform(hpx::execution::par, rng2, res.begin(), [](auto const& elem) {return elem * 2; });
#endif
			for (auto elem : rng2) {
				sum += elem;
			}
			auto end2 = std::chrono::high_resolution_clock::now();

			// don't consider first 5 iterations
			if (NUM_ITERATIONS < 5)
			{
				continue;
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
		std::cout << "SEQ: " << seqTime << '\n';
		std::cout << "PAR: " << parTime << '\n';
		std::cout << "RANGES: " << rangesTime << "\n\n";
	}

#if defined(UNIQUE_REPLACE_IF_FOR)
	std::ofstream outputFile("unique_replace_if_for_each.csv");
#elif defined(UNIQUE_REMOVE_IF_FOR)
	std::ofstream outputFile("unique_remove_if_for_each.csv");
#elif defined(REVERSE_REPLACE_IF_FOR)
	std::ofstream outputFile("reverse_replace_if_for_each.csv");
#elif defined(REVERSE_REMOVE_IF_FOR)
	std::ofstream outputFile("reverse_remove_if_for_each.csv");
#elif defined(FOR_EACH_TRANSFORM)
	std::ofstream outputFile("for_each_transform.csv");
#else
	std::ofstream outputFile("rangeAlgo.csv");
#endif
	for (auto& d : data)
	{
		outputFile << d[0] << "," << d[1] << "," << d[2] << "," << d[3] << ","
			<< ",\n";
	}
	return 0;
}