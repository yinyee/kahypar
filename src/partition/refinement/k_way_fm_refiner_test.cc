/***************************************************************************
 *  Copyright (C) 2015 Sebastian Schlag <sebastian.schlag@kit.edu>
 **************************************************************************/

#include "gmock/gmock.h"

#include "lib/definitions.h"
#include "partition/refinement/KWayFMRefiner.h"
#include "partition/refinement/policies/FMStopPolicies.h"

using::testing::Test;
using::testing::Eq;

using defs::Hypergraph;
using defs::HyperedgeIndexVector;
using defs::HyperedgeVector;
using defs::HyperedgeWeight;
using defs::HypernodeID;

namespace partition {
using KWayFMRefinerSimpleStopping = KWayFMRefiner<NumberOfFruitlessMovesStopsSearch>;


class AKwayFMRefiner : public Test {
 public:
  AKwayFMRefiner() :
    config(),
    hypergraph(new Hypergraph(2, 2, HyperedgeIndexVector { 0, 2,  /*sentinel*/ 3 },
                              HyperedgeVector { 0, 1, 0 }, 2)),
    refiner() {
    config.two_way_fm.max_number_of_fruitless_moves = 50;
    config.partition.total_graph_weight = 2;
    config.partition.k = 2;
    config.partition.epsilon = 1.0;
    config.partition.max_part_weight =
      (1 + config.partition.epsilon)
      * ceil(hypergraph->numNodes() / static_cast<double>(config.partition.k));

    hypergraph->setNodePart(0, 0);
    hypergraph->setNodePart(1, 1);

    refiner = std::make_unique<KWayFMRefinerSimpleStopping>(*hypergraph, config);
    refiner->initialize(100);
  }

  Configuration config;
  std::unique_ptr<Hypergraph> hypergraph;
  std::unique_ptr<KWayFMRefinerSimpleStopping> refiner;
};

TEST_F(AKwayFMRefiner, ConsidersSingleNodeHEsDuringInitialGainComputation) {
  refiner->insertHNintoPQ(0, 10);

  ASSERT_THAT(refiner->_pq.key(0, 1), Eq(1));
}

TEST_F(AKwayFMRefiner, ConsidersSingleNodeHEsDuringInducedGainComputation) {
  ASSERT_THAT(refiner->gainInducedByHypergraph(0, 0), Eq(1));
}
}  // namespace partition