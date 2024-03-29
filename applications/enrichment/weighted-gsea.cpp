#include <genetrail2/enrichment/common.h>
#include <genetrail2/enrichment/CommandLineInterface.h>
#include <genetrail2/enrichment/EnrichmentAlgorithm.h>
#include <genetrail2/enrichment/Parameters.h>

#include <genetrail2/core/EntityDatabase.h>
#include <genetrail2/core/GeneSet.h>

#include <iostream>
#include <memory>

using namespace GeneTrail;
namespace bpo = boost::program_options;
namespace bm = boost::math;

bool increasing = false, absolute = false;

bool parseArguments(int argc, char* argv[], Params& p)
{
	bpo::variables_map vm;
	bpo::options_description desc;

	addCommonCLIArgs(desc, p);
	desc.add_options()("identifier, d", bpo::value(&p.identifier_), "A file containing identifier line by line.")(
	                   "increasing,i", bpo::value(&increasing)->zero_tokens(), "Use increasingly sorted scores. (Decreasing is default)")(
	                   "absolute,abs", bpo::value(&absolute)->zero_tokens(), "Use decreasingly sorted absolute scores.");

	if(absolute && increasing) {
		std::cerr << "ERROR: Please specify only one option to sort the file."
		          << "\n";
	}

	try
	{
		bpo::store(bpo::command_line_parser(argc, argv).options(desc).run(),
		           vm);
		bpo::notify(vm);
	}
	catch(bpo::error& e)
	{
		std::cerr << "ERROR: " << e.what() << "\n";
		desc.print(std::cerr);
		return false;
	}

	return checkCLIArgs(p);
}

int main(int argc, char* argv[])
{
	Params p;
	if(!parseArguments(argc, argv, p)) {
		return -1;
	}

	GeneSet test_set;
	CategoryList cat_list;
	if(init(test_set, cat_list, p) != 0) {
		return -1;
	}

	auto db = std::make_shared<EntityDatabase>();
	Scores scores(test_set, db);

	auto order = increasing ? Order::Increasing : Order::Decreasing;

	auto algorithm = createEnrichmentAlgorithm<WeightedKolmogorovSmirnov>(p.pValueMode, scores, order);

	run(scores, cat_list, algorithm, p, true);
	return 0;
}
