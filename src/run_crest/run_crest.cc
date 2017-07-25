// Copyright (c) 2008, Jacob Burnim (jburnim@cs.berkeley.edu)
//
// This file is part of CREST, which is distributed under the revised
// BSD license.  A copy of this license can be found in the file LICENSE.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See LICENSE
// for details.

#include <assert.h>
#include <stdio.h>
#include <sys/time.h>

#include "run_crest/concolic_search.h"

//
// hEdit: add 3 arguments to the tool, i.e., *comm_world_size*
// and *num_params* to fit the need of MPI program
//
int main(int argc, char* argv[]) {
	if (argc < 6) {
		fprintf(stderr, "Syntax: run_crest <program> "
				"<Total number of ranks> "
				"<number of parameters> "
				"<number of iterations> "
				"-<strategy> [strategy options]\n");
		fprintf(stderr, "  Strategies include: "
				"dfs, cfg, random, uniform_random, random_input \n");
		return 1;
	}

	string prog = argv[1];
	int comm_world_size = atoi(argv[2]);
	//int rank_id = atoi(argv[3]);
	int num_params = atoi(argv[3]);
	int num_iters = atoi(argv[4]);
	string search_type = argv[5];

	// Initialize the random number generator.
	struct timeval tv;
	gettimeofday(&tv, NULL);
	srand((tv.tv_sec * 1000000) + tv.tv_usec);

	crest::Search* strategy;
	if (search_type == "-random") {
		strategy = new crest::RandomSearch(prog, num_iters, comm_world_size,
				num_params);
	} else if (search_type == "-random_input") {
		strategy = new crest::RandomInputSearch(prog, num_iters, comm_world_size,
				num_params);
	} else if (search_type == "-dfs") {
		if (argc == 6) {
			strategy = new crest::BoundedDepthFirstSearch(prog, num_iters,
					comm_world_size, num_params, 1000000);
		} else {
			strategy = new crest::BoundedDepthFirstSearch(prog, num_iters,
					comm_world_size, num_params, atoi(argv[6]));
		}
	} else if (search_type == "-cfg") {
		strategy = new crest::CfgHeuristicSearch(prog, num_iters, comm_world_size,
				num_params);
	} else if (search_type == "-cfg_baseline") {
		strategy = new crest::CfgBaselineSearch(prog, num_iters, comm_world_size,
				num_params);
	} else if (search_type == "-hybrid") {
		strategy = new crest::HybridSearch(prog, num_iters, comm_world_size,
				num_params, 100);
	} else if (search_type == "-uniform_random") {
		if (argc == 6) {
			strategy = new crest::UniformRandomSearch(prog, num_iters, comm_world_size,
					num_params, 100000000);
		} else {
			strategy = new crest::UniformRandomSearch(prog, num_iters, comm_world_size,
					num_params, atoi(argv[6]));
		}
	} else {
		fprintf(stderr, "Unknown search strategy: %s\n", search_type.c_str());
		return 1;
	}

	strategy->Run();

	delete strategy;

	return 0;
}