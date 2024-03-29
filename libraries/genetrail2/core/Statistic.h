/*
 * GeneTrail2 - An efficient library for interpreting genetic data
 * Copyright (C) 2013 Tim Kehl tkehl@bioinf.uni-sb.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the Lesser GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Lesser GNU General Public License for more details.
 *
 * You should have received a copy of the Lesser GNU General Public
 * License along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef GT2_CORE_STATISTIC_H
#define GT2_CORE_STATISTIC_H

#include <algorithm>
#include <cmath>
#include <numeric>
#include <tuple>
#include <vector>

namespace GeneTrail
{

	/**
	 * A collection of mathematical operations.
	 */
	namespace statistic
	{
		/**
		 * This method calculates the absolute value for each entry of a given
		 * range. The results are updated in place.
		 *
		 * @param begin InputIterator corresponding to the start of the samples.
		 * @param end   InputIterator corresponding to the end of the samples.
		 */
		template <typename value_type, typename InputIterator>
		void abs(InputIterator begin, InputIterator end)
		{
			std::transform(begin, end, begin,
			               static_cast<value_type (*)(value_type)>(std::abs));
		}

		/**
		 * This method calculates the square root for each entry of a given
		 * range. The results are updated in place.
		 *
		 * @param begin InputIterator corresponding to the start of the samples.
		 * @param end   InputIterator corresponding to the end of the samples.
		 */
		template <typename value_type, typename InputIterator>
		void sqrt(InputIterator begin, InputIterator end)
		{
			std::transform(begin, end, begin,
			               static_cast<value_type (*)(value_type)>(std::sqrt));
		}

		/**
		 * This method calculates the natural logarithm for each entry of a
		 * given range. The results are updated in place.
		 *
		 * @param begin InputIterator corresponding to the start of the samples.
		 * @param end   InputIterator corresponding to the end of the samples.
		 */
		template <typename value_type, typename InputIterator>
		void log(InputIterator begin, InputIterator end)
		{
			std::transform(begin, end, begin,
			               static_cast<value_type (*)(value_type)>(std::log));
		}

		/**
		 * This method calculates the logarithm (base 10) for each entry of a
		 * given range. The results are updated in place.
		 *
		 * @param begin InputIterator corresponding to the start of the samples.
		 * @param end   InputIterator corresponding to the end of the samples.
		 */
		template <typename value_type, typename InputIterator>
		void log10(InputIterator begin, InputIterator end)
		{
			std::transform(begin, end, begin,
			               static_cast<value_type (*)(value_type)>(std::log10));
		}

		/**
		 * This method calculates the logarithm (base 2) for each entry of a
		 * given range. The results are updated in place.
		 *
		 * @param begin InputIterator corresponding to the start of the samples.
		 * @param end   InputIterator corresponding to the end of the samples.
		 */
		template <typename value_type, typename InputIterator>
		void log2(InputIterator begin, InputIterator end)
		{
			std::transform(begin, end, begin,
			               static_cast<value_type (*)(value_type)>(std::log2));
		}

		/**
		 * This method calculates the n-th power for each entry of a given
		 * range. The results are updated in place.
		 * @param begin InputIterator corresponding to the start of the samples.
		 * @param end   InputIterator corresponding to the end of the samples.
		 */
		template <typename value_type, typename InputIterator>
		void pow(InputIterator begin, InputIterator end, int n)
		{
			std::transform(begin, end, begin,
			               [n](value_type x) { return std::pow(x, n); });
		}

		/**
		 * This method calculates the maximal value of a given range.
		 *
		 * @param begin InputIterator corresponding to the start of the samples.
		 * @param end   InputIterator corresponding to the end of the samples.
		 *
		 * @return Maximum of the given range
		 */
		template <typename value_type, typename InputIterator>
		value_type max(InputIterator begin, InputIterator end)
		{
			return *std::max_element(begin, end);
		}

		/**
		 * This method calculates the minimal value of a given range.
		 *
		 * @param begin InputIterator corresponding to the start of the samples.
		 * @param end   InputIterator corresponding to the end of the samples.
		 *
		 * @return Minimum of the given range
		 */
		template <typename value_type, typename InputIterator>
		value_type min(InputIterator begin, InputIterator end)
		{
			return *std::min_element(begin, end);
		}

		/**
		 * This method calculates the sum of a given range.
		 *
		 * @param begin InputIterator corresponding to the start of the samples.
		 * @param end   InputIterator corresponding to the end of the samples.
		 *
		 * @return Sum of the given range
		 */
		template <typename value_type, typename InputIterator>
		value_type sum(InputIterator begin, InputIterator end)
		{
			return std::accumulate(begin, end, 0.0);
		}

		/**
		 * This method calculates the mean of a given range.
		 *
		 * @param begin InputIterator corresponding to the start of the samples.
		 * @param end   InputIterator corresponding to the end of the samples.
		 *
		 * @return Mean of the given range
		 */
		template <typename value_type, typename InputIterator>
		double mean(InputIterator begin, InputIterator end)
		{
			auto dist = std::distance(begin, end);
			if(dist == 0){
				return value_type();
			}

			return (std::accumulate(begin, end, 0.0) / ((value_type)dist));
		}

		/**
		 * This method calculates the max-mean statistic of a given range.
		 *
		 * @param begin InputIterator corresponding to the start of the samples.
		 * @param end   InputIterator corresponding to the end of the samples.
		 *
		 * @return Max-mean statistic of the given range
		 */
		template <typename value_type, typename InputIterator>
		value_type max_mean(InputIterator begin, InputIterator end)
		{
			value_type n = std::distance(begin, end);

			if(n == 0){
				return value_type();
			}

			value_type positive_sum = 0;
			value_type negative_sum = 0;
			for(auto iter = begin; iter != end; ++iter) {
				if(*iter < 0) {
					negative_sum -= *iter;
				} else {
					positive_sum += *iter;
				}
			}

			// In the original paper, they return absolute values.
			// But as we want to distinguish between positive and negative,
			// we return signed values.
			if(negative_sum > positive_sum) {
				return -negative_sum / n;
			} else {
				return positive_sum / n;
			}
		}

		/**
		 * This method calculates the median of a given range.
		 *
		 * @param begin InputIterator corresponding to the start of the samples.
		 * @param end   InputIterator corresponding to the end of the samples.
		 *
		 * @return Median of the given range
		 */
		template <typename value_type, typename InputIterator>
		value_type median(InputIterator begin, InputIterator end)
		{
			std::vector<value_type> tmp(begin, end);

			const auto dist = std::distance(tmp.begin(), tmp.end());

			if(dist == 0) {
				return value_type();
			}

			const auto dist2 = dist / 2;
			auto median_position = tmp.begin() + dist2;
			std::nth_element(tmp.begin(), median_position, tmp.end());

			if(2 * dist2 == dist) {
				auto second = std::max_element(tmp.begin(), median_position);
				return (*median_position + *second) * value_type(0.5);
			} else {
				return *median_position;
			}
		}

		/**
		 * This method calculates the pooled variance of a given range.
		 *
		 * @param begin InputIterator corresponding to the start of the samples.
		 * @param end   InputIterator corresponding to the end of the samples.
		 *
		 * @return Variance of the given range
		 */
		template <typename value_type, typename InputIterator>
		value_type var(InputIterator begin, InputIterator end, value_type mean)
		{
			int n = std::distance(begin, end);
			if(n <= 1) {
				return ((value_type)0);
			} else {
				value_type v = value_type();
				value_type ep = value_type();
				for(auto it = begin; it != end; ++it) {
					value_type diff = *it - mean;
					ep += diff;
					v += diff * diff;
				}
				return (v - ep * ep / n) / (n - 1);
			}
		}

		/**
		 * This method calculates the pooled variance of a given range.
		 *
		 * @param begin InputIterator corresponding to the start of the samples
		 * @param end   InputIterator corresponding to the end of the samples
		 * @return Variance of the given range
		 */
		template <typename value_type, typename InputIterator>
		value_type var(InputIterator begin, InputIterator end)
		{
			int n = std::distance(begin, end);
			if(n <= 1) {
				return ((value_type)0);
			} else {
				const value_type s = std::accumulate(begin, end, value_type());
				const value_type mean = s / n;
				return var(begin, end, mean);
			}
		}

		/**
		 * This implements the online algorithm by Knuth for calculating the variance.
		 * The advantage of this method is, that every input value needs to be read only
		 * once. It additionally computes the mean and the number of input samples.
		 *
		 * For more information see:
		 * Donald Knuth, The Art of Computer Programming, Vol 2, page 232, 3rd edition.
		 *
		 * @param begin InputIterator pointing to the start of a range for which the
		 *              variance should be computed.
		 * @param end   InputIterator pointing at the end of the range.
		 */
		template<typename value_type, typename InputIterator>
		std::tuple<value_type, value_type, size_t> mean_var_size(InputIterator begin, InputIterator end)
		{
			value_type mean = 0.0;
			value_type var = 0.0;
			size_t size = 0u;

			for(; begin != end; ++begin) {
				auto x = *begin;
				++size;
				auto delta = x - mean;
				mean += delta / size;
				var += delta * (x - mean);
			}

			if(size <= 1) {
				return std::make_tuple(mean, value_type(), size);
			}

			return std::make_tuple(mean, var / (size - 1), size);
		}

		/**
		 * This method calculates the standard deviation of a given range.
		 *
		 * @param begin InputIterator corresponding to the start of the samples
		 * @param end   InputIterator corresponding to the end of the samples
		 * @return Standard deviation of the given range
		 */
		template <typename value_type, typename InputIterator>
		value_type sd(InputIterator begin, InputIterator end)
		{
			return std::sqrt(var<value_type, InputIterator>(begin, end));
		}

		/**
		 * This method calculates the sample covariance between two ranges of
		 * values.
		 *
		 * @param first_begin  InputIterator corresponding to the start of the first group.
		 * @param first_end    InputIterator corresponding to the end of the first group.
		 * @param second_begin InputIterator corresponding to the start of the second group.
		 * @param second_end   InputIterator corresponding to the end of the second group.
		 *
		 * @return Covariance of the given range
		 */
		template <typename value_type, typename InputIterator>
		value_type cov(InputIterator first_begin, InputIterator first_end,
		               InputIterator second_begin, InputIterator second_end)
		{
			assert(std::distance(first_begin, first_end) ==
			       std::distance(second_begin, second_end));
			value_type mean1 =
			    mean<value_type, InputIterator>(first_begin, first_end);
			value_type mean2 =
			    mean<value_type, InputIterator>(second_begin, second_end);
			value_type cov = 0.0;
			size_t n = std::distance(first_begin, first_end);
			for(size_t i = 0; i < n; ++i) {
				cov += (*(first_begin + i) - mean1) *
				       (*(second_begin + i) - mean2);
			}
			return cov / (n - 1);
		}

		/**
		 * This methods implements Pearson's correlation coefficient.
		 *
		 * @param first_begin  InputIterator corresponding to the start of the first group.
		 * @param first_end    InputIterator corresponding to the end of the first group.
		 * @param second_begin InputIterator corresponding to the start of the second group.
		 * @param second_end   InputIterator corresponding to the end of the second group.
		 *
		 * @return Correlation coefficient
		 */
		template <typename value_type, typename InputIterator>
		value_type pearson_correlation(InputIterator first_begin,
		                               InputIterator first_end,
		                               InputIterator second_begin,
		                               InputIterator second_end)
		{
			assert(std::distance(first_begin, first_end) ==
			       std::distance(second_begin, second_end));
			value_type covar = cov<value_type, InputIterator>(
			    first_begin, first_end, second_begin, second_end);
			value_type sd1 =
			    sd<value_type, InputIterator>(first_begin, first_end);
			value_type sd2 =
			    sd<value_type, InputIterator>(second_begin, second_end);
			return covar / (sd1 * sd2);
		}

		/**
		 */
		template <typename value_type, typename InputIterator>
		std::vector<int> ranks(InputIterator begin, InputIterator end)
		{
			std::vector<value_type> tmp(begin, end);
			std::sort(tmp.begin(), tmp.end());
			std::vector<int> ranks;
			for(auto it = begin; it != end; ++it) {
				for(size_t i = 0; i < tmp.size(); ++i) {
					if(*it == *(tmp.begin() + i)) {
						ranks.push_back(i);
					}
				}
			}
			return ranks;
		}

		/**
		 * This methods implements Spearman's correlation coefficient.
		 *
		 * @param first_begin  InputIterator corresponding to the start of the first group.
		 * @param first_end    InputIterator corresponding to the end of the first group.
		 * @param second_begin InputIterator corresponding to the start of the second group.
		 * @param second_end   InputIterator corresponding to the end of the second group.
		 *
		 * @return Correlation coefficient
		 */
		template <typename value_type, typename InputIterator>
		value_type spearman_correlation(InputIterator first_begin,
		                                InputIterator first_end,
		                                InputIterator second_begin,
		                                InputIterator second_end)
		{
			std::vector<int> first_ranks =
			    ranks<value_type, InputIterator>(first_begin, first_end);
			std::vector<int> second_ranks =
			    ranks<value_type, InputIterator>(second_begin, second_end);
			return pearson_correlation<value_type, std::vector<int>::iterator>(
			    first_ranks.begin(), first_ranks.end(), second_ranks.begin(),
			    second_ranks.end());
		}

		/**
		 * This function calculates log(mean(a)) -  log(mean(b)).
		 * Log-Mean-Fold-Quotient
		 *
		 * @param begin1 InputIterator corresponding to the start of the first group.
		 * @param end1   InputIterator corresponding to the end of the first group.
		 * @param begin2 InputIterator corresponding to the start of the second group.
		 * @param end2   InputIterator corresponding to the end of the second group.
		 *
		 * @return log(mean(a)) -  log(mean(b))
		 */
		template <typename value_type, typename InputIterator1,
		          typename InputIterator2>
		value_type
		log_mean_fold_quotient(InputIterator1 begin1, InputIterator1 end1,
		                       InputIterator2 begin2, InputIterator2 end2)
		{
			return std::log(mean<value_type, InputIterator1>(begin1, end1)) -
			       std::log(mean<value_type, InputIterator2>(begin2, end2));
		}

		/**
		 * Log-Median-Fold-Quotient
		 * This function calculates log(median(a)) -  log(median(b)).
		 *
		 * @param begin1 InputIterator corresponding to the start of the first group.
		 * @param end1   InputIterator corresponding to the end of the first group.
		 * @param begin2 InputIterator corresponding to the start of the second group.
		 * @param end2   InputIterator corresponding to the end of the second group.
		 *
		 * @return log(median(a)) -  log(median(b))
		 */
		template <typename value_type, typename InputIterator1,
		          typename InputIterator2>
		value_type
		log_median_fold_quotient(InputIterator1 begin1, InputIterator1 end1,
		                         InputIterator2 begin2, InputIterator2 end2)
		{
			return std::log(median<value_type, InputIterator1>(begin1, end1)) -
			       std::log(median<value_type, InputIterator2>(begin2, end2));
		}

		/**
		 * This function calculates mean(a) /  mean(b).
		 * Mean-Fold-Quotient
		 *
		 * @param begin1 InputIterator corresponding to the start of the first group.
		 * @param end1   InputIterator corresponding to the end of the first group.
		 * @param begin2 InputIterator corresponding to the start of the second group.
		 * @param end2   InputIterator corresponding to the end of the second group.
		 *
		 * @return mean(a) /  mean(b)
		 */
		template <typename value_type, typename InputIterator1,
		          typename InputIterator2>
		value_type
		mean_fold_quotient(InputIterator1 begin1, InputIterator1 end1,
		                   InputIterator2 begin2, InputIterator2 end2)
		{
			return mean<value_type, InputIterator1>(begin1, end1) /
			       mean<value_type, InputIterator2>(begin2, end2);
		}

		/**
		* This function calculates mean(a) -  mean(b).
		* Mean-Fold-Difference
		*
		 * @param begin1 InputIterator corresponding to the start of the first group.
		 * @param end1   InputIterator corresponding to the end of the first group.
		 * @param begin2 InputIterator corresponding to the start of the second group.
		 * @param end2   InputIterator corresponding to the end of the second group.
		 *
		* @return mean(a) -  mean(b)
		*/
		template <typename value_type, typename InputIterator1,
		          typename InputIterator2>
		value_type
		mean_fold_difference(InputIterator1 begin1, InputIterator1 end1,
		                     InputIterator2 begin2, InputIterator2 end2)
		{
			return mean<value_type, InputIterator1>(begin1, end1) -
			       mean<value_type, InputIterator2>(begin2, end2);
		}

		/**
		 * Median-Fold-Quotient
		 * This function calculates median(a) /  median(b).
		 *
		 * @param begin1 InputIterator corresponding to the start of the first group.
		 * @param end1   InputIterator corresponding to the end of the first group.
		 * @param begin2 InputIterator corresponding to the start of the second group.
		 * @param end2   InputIterator corresponding to the end of the second group.
		 *
		 * @return median(a) /  median(b)
		 */
		template <typename value_type, typename InputIterator1,
		          typename InputIterator2>
		value_type
		median_fold_quotient(InputIterator1 begin1, InputIterator1 end1,
		                     InputIterator2 begin2, InputIterator2 end2)
		{
			return median<value_type, InputIterator1>(begin1, end1) /
			       median<value_type, InputIterator2>(begin2, end2);
		}

		/**
		 * Mean-Fold-Difference
		 * This function calculates median(a) -  median(b).
		 *
		 * @param begin1 InputIterator corresponding to the start of the first group.
		 * @param end1   InputIterator corresponding to the end of the first group.
		 * @param begin2 InputIterator corresponding to the start of the second group.
		 * @param end2   InputIterator corresponding to the end of the second group.
		 *
		 * @return median(a) -  median(b)
		 */
		template <typename value_type, typename InputIterator1,
		          typename InputIterator2>
		value_type
		median_fold_difference(InputIterator1 begin1, InputIterator1 end1,
		                       InputIterator2 begin2, InputIterator2 end2)
		{
			return median<value_type, InputIterator1>(begin1, end1) -
			       median<value_type, InputIterator2>(begin2, end2);
		}

		/**
		 * This function calculates the z-score.
		 *
		 * @param x Input for which the z-score is requested
		 * @param begin InputIterator corresponding to the start of the samples
		 * @param end   InputIterator corresponding to the end of the samples
		 *
		 * @return Z-score
		 */
		template <typename value_type, typename InputIterator1>
		value_type z_score(value_type x, InputIterator1 begin,
		                   InputIterator1 end)
		{
			return (x - mean<value_type, InputIterator1>(begin, end)) /
			       sd<value_type, InputIterator1>(begin, end);
		}
	}
}

#endif // GT2_CORE_STATISTIC_H
