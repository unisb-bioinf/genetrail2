/*
 * GeneTrail2 - An efficient library for interpreting genetic data
 * Copyright (C) 2015 Daniel Stöckel <dstoeckel@bioinf.uni-sb.de>
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
#include "Scores.h"
#include "Category.h"
#include "GeneSet.h"

#include <algorithm>

namespace GeneTrail
{
	Scores::IndexProxy::IndexProxy(const std::vector<Score>* data) : data_(data)
	{
	}

	Scores::ConstScoresProxy::ConstScoresProxy(const std::vector<Score>* data)
	    : data_(data)
	{
	}

	Scores::ScoresProxy::ScoresProxy(std::vector<Score>* data)
	    : data_(data)
	{
	}

	Scores::NamesProxy::NamesProxy(const std::vector<Score>* data,
	                               const EntityDatabase* db)
	    : data_(data), db_(db)
	{
	}

	bool Scores::LessScore::operator()(const Score& a, const Score& b) const
	{
		if(a.score() == b.score()) {
			return a.index() < b.index();
		}

		return a.score() < b.score();
	}

	bool Scores::GreaterScore::operator()(const Score& a, const Score& b) const
	{
		if(a.score() == b.score()) {
			return a.index() > b.index();
		}

		return a.score() > b.score();
	}

	bool Scores::LessIndex::operator()(const Score& a, const Score& b) const
	{
		return a.index() < b.index();
	}

	Scores::Scores(const std::vector<Score>& data,
	               const std::shared_ptr<EntityDatabase>& db)
	    : data_(data), isSortedByIndex_(false), db_(db)
	{
	}

	Scores::Scores(std::vector<Score>&& data,
	               const std::shared_ptr<EntityDatabase>& db)
	    : data_(std::move(data)), isSortedByIndex_(false), db_(db)
	{
	}

	Scores::Scores(const GeneTrail::GeneSet& gene_set,
	               const std::shared_ptr<EntityDatabase>& db)
	    : isSortedByIndex_(true), db_(db)
	{
		data_.reserve(gene_set.size());

		// Insert the entries of the gene set. emplace_back
		// ensures, that isSortedByName_ is updated properly.
		for(const auto& entry : gene_set) {
			emplace_back(entry.first, entry.second);
		}
	}

	Scores::Scores(GeneSet&& gene_set,
	               const std::shared_ptr<EntityDatabase>& db)
	    : isSortedByIndex_(true), db_(db)
	{
		data_.reserve(gene_set.size());

		// Insert the entries of the gene set. emplace_back
		// ensures, that isSortedByName_ is updated properly.
		for(auto& entry : gene_set) {
			emplace_back(std::move(entry.first), entry.second);
		}
	}

	Scores::Scores(const std::shared_ptr<EntityDatabase>& db)
	    : isSortedByIndex_(true), db_(db)
	{
	}
	Scores::Scores(size_t size, const std::shared_ptr<EntityDatabase>& db)
	    : isSortedByIndex_(true), db_(db)
	{
		data_.reserve(size);
	}

	Scores Scores::subset(const Category& c) const
	{
		if(isSortedByIndex_) {
			return subsetMerge_(c);
		}

		return subsetFind_(c);
	}

	Scores Scores::subsetMerge_(const Category& c) const
	{
		auto n = size();
		Scores result(std::min(n, c.size()), db_);

		auto scoresIt = begin();
		auto categoryIt = c.begin();

		while(scoresIt != end() && categoryIt != c.end()) {
			auto search_end = end();

			// This is a heuristic that tries to guess the position
			// of the current category element in the scores vector
			// For this purpose we assume that the scores vector contains
			// almost all identifiers in the entity database and we can simply
			// perform a lookup in the scores vector to get an approximate
			// location
			if(*categoryIt < n) {
				// Look up the score index that can be found at the
				// current index in the category.
				auto cat_lookup = begin() + *categoryIt;
				auto scores_index = cat_lookup->index();

				if(scores_index == *categoryIt) {
					// If we hit what we were looking for, we
					// are done and can continue.
					scoresIt = cat_lookup + 1;
					++categoryIt;
					result.emplace_back(*cat_lookup);
					continue;
				} else if(scores_index > *categoryIt) {
					// If the found index is larger than what we were looking
					// for move the end of the search range here.
					search_end = cat_lookup;
				} else {
					// Otherwise the searched element must be somewhere behind
					// the examined position.
					scoresIt = cat_lookup;
				}
			}

			// TODO: Temporary can be optimized away using C++14 heterogenous
			//      lookup.
			Score dummy(*categoryIt, 0.0);
			scoresIt = std::lower_bound(scoresIt, search_end, dummy, LessIndex());

			// Check that we found the index we searched for
			if(scoresIt != search_end) {
				result.emplace_back(*scoresIt);
				++scoresIt;
			}
			++categoryIt;
		}

		return result;
	}

	Scores Scores::subsetFind_(const Category& c) const
	{
		assert(db_.get() == c.entityDatabase());

		Scores result(std::min(size(), c.size()), db_);

		for(const auto& entry : *this) {
			if(c.contains(entry.index())) {
				result.emplace_back(entry);
			}
		}

		return result;
	}

	std::vector<size_t> Scores::subsetIndices(const Category& c) const
	{
		std::vector<size_t> result;
		result.reserve(std::min(size(), c.size()));

		for(size_t i = 0; i < size(); ++i) {
			if(c.contains(data_[i].index())) {
				result.emplace_back(i);
			}
		}

		return result;
	}

	void Scores::sortByIndex()
	{
		// Nothing to do here
		if(isSortedByIndex_) {
			return;
		}

		isSortedByIndex_ = true;

		std::sort(data_.begin(), data_.end(), LessIndex());
	}

	void Scores::sortByName()
	{
		isSortedByIndex_ = size() <= 1;

		std::sort(data_.begin(), data_.end(),
		          [this](const Score& a, const Score& b) {
			return a.name(*db_) < b.name(*db_);
		});
	}

	void Scores::sortByScore(Order order)
	{
		// The data is only sorted by name if there is at most one item present.
		isSortedByIndex_ = size() <= 1;

		switch(order) {
			case Order::Increasing:
				std::sort(data_.begin(), data_.end(), LessScore());
				break;
			case Order::Decreasing:
				std::sort(data_.begin(), data_.end(), GreaterScore());
				break;
		}
	}

	bool Scores::contains(const std::string& name) const
	{
		// TODO: This might be replaced with heterogenous lookup from
		//      C++14
		return contains(Score(*db_, name, 0.0));
	}

	bool Scores::contains(const Score& score) const
	{
		if(isSortedByIndex_) {
			return std::binary_search(data_.begin(), data_.end(), score, LessIndex());
		} else {
			auto pred = [&score](const Score& a) {
				return a.index() == score.index();
			};

			return std::find_if(data_.begin(), data_.end(), pred) !=
		           data_.end();
		}
	}
}
