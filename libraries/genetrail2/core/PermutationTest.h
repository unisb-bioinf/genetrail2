#ifndef GT2_CORE_PERMUTATION_TEST_H
#define GT2_CORE_PERMUTATION_TEST_H

#include "macros.h"
#include "DenseMatrix.h"
#include "DenseMatrixSubset.h"
#include "EnrichmentAlgorithm.h"
#include "MatrixHTest.h"

#include <boost/iterator/counting_iterator.hpp>

#include <algorithm>
#include <vector>
#include <utility>
#include <random>
#include <iostream>

namespace GeneTrail
{
	namespace internal
	{
		template <typename value_type> class PermutationBase
		{
			protected:
			double computePValue_(size_t permutations, size_t counter) const
			{
				// Here we add a pseudo count to avoid p-values of 0.
				// Reference: Fewer permutations, more accurate P-values.
				// http://www.ncbi.nlm.nih.gov/pmc/articles/PMC2687965/

				if(permutations == 0) {
					return 1;
				}

				return ((double)counter + 1) / ((double)permutations);
			}

			void sortResults_(EnrichmentResults& results)
			{
				// Sort vector of EnrichmentResults
				// This makes computation faster
				std::sort(results.begin(), results.end(),
				          [](const EnrichmentResultPtr& a,
				             const EnrichmentResultPtr& b) {
					return a->hits < b->hits;
				});
			}

			void updateCounter_(const EnrichmentResultPtr& test,
			                    size_t& counter, double score)
			{
				if(test->enriched) {
					if(test->score <= score) {
						++counter;
					}
				} else {
					if(test->score >= score) {
						++counter;
					}
				}
			}

			void updatePValues_(EnrichmentResults& tests,
			                    const std::vector<size_t>& counter,
			                    size_t permutations)
			{
				for(size_t i = 0; i < tests.size(); ++i) {
					tests[i]->pvalue = computePValue_(permutations, counter[i]);
				}
			}
		};
	}

	template <typename value_type>
	class GT2_EXPORT RowPermutationTest
	    : public internal::PermutationBase<value_type>
	{
		public:
		static std::unique_ptr<RowPermutationTest>
		IndexBased(const Scores& s, size_t permutations,
		               uint64_t randomSeed)
		{
			return std::unique_ptr<RowPermutationTest>(new RowPermutationTest(
			    boost::counting_iterator<size_t>(0),
			    boost::counting_iterator<size_t>(s.size()), permutations,
			    randomSeed));
		}

		static std::unique_ptr<RowPermutationTest>
		CategoryBased(const Scores& s, size_t permutations,
		                  uint64_t randomSeed)
		{
			return std::unique_ptr<RowPermutationTest>(new RowPermutationTest(
			    s.indices().begin(), s.indices().end(), permutations,
			    randomSeed));
		}

		void computePValue(const EnrichmentAlgorithmPtr& algorithm,
		                   EnrichmentResults& tests)
		{
			if(tests.empty()) {
				return;
			}

			this->sortResults_(tests);

			std::vector<size_t> counter(tests.size());
			for(size_t i = 0; i < permutations_; ++i) {
				std::cout << "INFO: Running - Permutation test " << (i + 1)
				          << "/" << permutations_ << std::endl;

				performSinglePermutation_(algorithm, tests, counter);
			}

			this->updatePValues_(tests, counter, permutations_);
		}

		private:
		template <typename InputIterator>
		RowPermutationTest(InputIterator begin, InputIterator end,
		                   size_t permutations, uint64_t randomSeed)
		    : permutations_(permutations),
		      twister_(randomSeed),
		      indices_(begin, end),
		      tmp_indices_(std::distance(begin, end))
		{
		}

		std::tuple<double, double>
		computeEnrichmentScore_(const EnrichmentAlgorithmPtr& algorithm,
		                        size_t currentSampleSize)
		{
			auto start = indices_.begin();
			auto stop = indices_.begin() + currentSampleSize;

			if(algorithm->supportsIndices()) {
				return algorithm->computeEnrichmentScore(start, stop);
			} else {
				category_.replaceAll(start, stop);
				return algorithm->computeEnrichmentScore(category_);
			}
		}

		void performSinglePermutation_(const EnrichmentAlgorithmPtr& algorithm,
		                               const EnrichmentResults& tests,
		                               std::vector<size_t>& counter)
		{
			size_t currentSampleSize = 0;
			value_type currentScore = 0.0;

			// Shuffle the indices. The tests are sorted
			// by the number of hits for every category, so we only
			// need to shuffle tests.back()->hits many.
			shuffle_(tests.back()->hits);

			for(size_t i = 0; i < tests.size(); ++i) {
				// Check if the sampleSize has changed. As the tests_ vector is
				// sorted we can use one running sum value for all categories of
				// the same size.
				if(tests[i]->hits != currentSampleSize) {
					presort_(currentSampleSize, tests[i]->hits);
					currentSampleSize = tests[i]->hits;

					currentScore = std::get<0>(
					    computeEnrichmentScore_(algorithm, currentSampleSize));
				}

				this->updateCounter_(tests[i], counter[i], currentScore);
			}
		}

		void shuffle_(size_t n)
		{
			for(size_t i = 0; i < n; ++i) {
				std::swap(indices_[i],
				          indices_[i + twister_() % (indices_.size() - i)]);
			}
		}

		/**
		 * This sorts the indices vector until position b.
		 * It is assumed, that the vector is sorted up to position a.
		 */
		void presort_(size_t a, size_t b)
		{
			// Sort [a, b)
			std::sort(indices_.begin() + a, indices_.begin() + b);

			// Merge [0, a) and [a, b) into a temporary vector.
			// Note that inplace_merge would reallocate a new vector
			// every time it is called.
			std::merge(indices_.begin(), indices_.begin() + a,
			           indices_.begin() + a, indices_.begin() + b,
			           tmp_indices_.begin());

			// Copy the sorted range [0, b) from the temporary vector
			// to the names_ vector
			std::copy(tmp_indices_.begin(), tmp_indices_.begin() + b,
			          indices_.begin());
		}

		Category category_;
		size_t permutations_;
		std::mt19937_64 twister_;
		std::vector<size_t> indices_;
		std::vector<size_t> tmp_indices_;
	};

	template <typename value_type>
	class ColumnPermutationTest : public internal::PermutationBase<value_type>
	{
		public:
		ColumnPermutationTest(const DenseMatrix& data, size_t permutations,
		                      size_t reference_size, const std::string& method,
		                      uint64_t randomSeed)
		    : permutations_(permutations),
		      data_(data),
		      reference_size_(reference_size),
		      method_(method),
		      twister_(randomSeed),
		      row_permutation_(data.rows())
		{
			assert(rowNamesStrictlySorted_(data));
		}

		void computePValue(const EnrichmentAlgorithmPtr& algorithm,
		                   EnrichmentResults& tests)
		{
			std::vector<size_t> counter(tests.size());
			std::vector<size_t> indices(data_.cols());
			std::iota(indices.begin(), indices.end(), static_cast<size_t>(0));

			for(size_t i = 1; i <= permutations_; ++i) {
				std::cout << "INFO: Running - Permutation test " << i << "/"
				          << permutations_ << std::endl;

				performSinglePermutation_(algorithm, tests, counter, indices);
			}

			this->updatePValues_(tests, counter, permutations_);
		}

		private:
		bool rowNamesStrictlySorted_(const DenseMatrix& data)
		{
			Matrix::index_type i = 0;

			for(auto j = i + 1; j < data.rows(); ++j, ++i) {
				if(data.rowName(i) >= data.rowName(j)) {
					return false;
				}
			}

			return true;
		};

		void performSinglePermutation_(const EnrichmentAlgorithmPtr& algorithm,
		                               const EnrichmentResults& tests,
		                               std::vector<size_t>& counter,
		                               std::vector<size_t>& indices)
		{
			MatrixHTest<DenseMatrixSubset> scoring;
			std::shuffle(indices.begin(), indices.end(), twister_);

			auto mid = indices.begin() + reference_size_;

			DenseMatrixSubset ref(&data_, row_permutation_.begin(),
			                      row_permutation_.end(), indices.begin(), mid);
			DenseMatrixSubset sam(&data_, row_permutation_.begin(),
			                      row_permutation_.end(), mid, indices.end());
			Scores scores(scoring.test(ref, sam, method_));

			algorithm->setScores(scores);

			for(size_t i = 0; i < tests.size(); ++i) {
				auto score = std::get<0>(
				    algorithm->computeEnrichmentScore(*tests[i]->category));

				this->updateCounter_(tests[i], counter[i], score);
			}
		}

		size_t permutations_;
		DenseMatrix data_;
		size_t reference_size_;
		std::string method_;
		std::mt19937 twister_;
		std::vector<size_t> row_permutation_;
	};
}

#endif // GT2_CORE_PERMUTATION_TEST_H
