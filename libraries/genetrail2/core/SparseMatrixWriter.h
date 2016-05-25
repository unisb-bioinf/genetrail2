/*
 * GeneTrail2 - An efficient library for interpreting genetic data
 * Copyright (C) 2013 Daniel Stöckel <dstoeckel@bioinf.uni-sb.de>
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

#ifndef GT2_SPARSE_MATRIX_WRITER_H
#define GT2_SPARSE_MATRIX_WRITER_H

#include "macros.h"
#include "MatrixWriter.h"

#include <ostream>
#include <vector>

namespace GeneTrail
{
	class Matrix;
	class SparseMatrix;
	class SparseMatrixSubset;

	class GT2_EXPORT SparseMatrixWriter : public MatrixWriter
	{
		public:
			void writeText(std::ostream& output, const SparseMatrix& matrix) const;
			void writeText(std::ostream& output, const SparseMatrixSubset& matrix) const;

			/**
			 * Reads a matrix from a binary file.
			 * 
			 * \see SparseMatrixReader::binaryRead_
			 */
			uint64_t writeBinary(std::ostream& output, const SparseMatrix& matrix) const;
			uint64_t writeBinary(std::ostream& output, const SparseMatrixSubset& matrix) const;

	private:
			uint64_t writeData_(std::ostream& output, const SparseMatrix& matrix) const;
			uint64_t writeData_(std::ostream& output, const SparseMatrixSubset& matrix) const;
	};
}

#endif //GT2_SPARSE_MATRIX_WRITER_H
