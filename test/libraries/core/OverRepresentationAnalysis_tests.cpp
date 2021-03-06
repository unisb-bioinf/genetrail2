#include <gtest/gtest.h>

#include <boost/lexical_cast.hpp>

#include <genetrail2/core/Category.h>
#include <genetrail2/core/EntityDatabase.h>
#include <genetrail2/core/OverRepresentationAnalysis.h>

using namespace GeneTrail;

const double TOLERANCE = 0.00001;

/*
 * Test-set < Reference-set
 */
TEST(OverRepresentationAnalysis, HypergeometricTestCase1) {
	EntityDatabase db;
	Category ref(&db);
	for(int i=0; i<50; ++i){
		ref.insert(boost::lexical_cast<std::string>(i));
	}
	Category cat(&db);
	for(int i=0; i<5; ++i){
		cat.insert(boost::lexical_cast<std::string>(i));
	}
	Category test(&db);
	for(int i=1; i<11; ++i){
		test.insert(boost::lexical_cast<std::string>(i));
	}
	OverRepresentationAnalysis ora(ref,test);
	EXPECT_NEAR(ora.computePValue(cat), 0.003964583 + 0.0001189375, TOLERANCE);
}

TEST(OverRepresentationAnalysis, HypergeometricTestCase2) {
	EntityDatabase db;
	Category ref(&db);
	for(int i=0; i<50; ++i){
		ref.insert(boost::lexical_cast<std::string>(i));
	}
	Category cat(&db);
	for(int i=0; i<5; ++i){
		cat.insert(boost::lexical_cast<std::string>(i));
	}
	Category test(&db);
	for(int i=4; i<14; ++i){
		test.insert(boost::lexical_cast<std::string>(i));
	}
	OverRepresentationAnalysis ora(ref,test);
	EXPECT_NEAR(ora.computePValue(cat), 0.3105628 + 0.4313372, TOLERANCE);
}

TEST(OverRepresentationAnalysis, FisherTestCase1) {
	EntityDatabase db;
	Category ref(&db);
	for(int i=0; i<14; ++i){
		ref.insert(boost::lexical_cast<std::string>(i));
	}
	Category cat(&db);
	for(int i=5; i<14; ++i){
		cat.insert(boost::lexical_cast<std::string>(i));
	}
	Category test(&db);
	for(int i=7; i<17; ++i){
		test.insert(boost::lexical_cast<std::string>(i));
	}
	OverRepresentationAnalysis ora(ref,test);
	FishersExactTest<unsigned int, double> fet;
	EXPECT_NEAR(ora.computePValue(cat), fet.upperTailedPValue(14,9,10,7), TOLERANCE);
}

TEST(OverRepresentationAnalysis, FisherTestCase2) {
	EntityDatabase db;
	Category ref(&db);
	for(int i=0; i<14; ++i){
		ref.insert(boost::lexical_cast<std::string>(i));
	}
	Category cat(&db);
	for(int i=5; i<14; ++i){
		cat.insert(boost::lexical_cast<std::string>(i));
	}
	Category test(&db);
	for(int i=13; i<23; ++i){
		test.insert(boost::lexical_cast<std::string>(i));
	}
	OverRepresentationAnalysis ora(ref,test);
	FishersExactTest<unsigned int, double> fet;
	EXPECT_NEAR(ora.computePValue(cat), fet.lowerTailedPValue(14,9,10,1), TOLERANCE);
}
