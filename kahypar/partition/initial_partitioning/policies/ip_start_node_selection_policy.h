/***************************************************************************
 *  Copyright (C) 2015 Tobias Heuer <tobias.heuer@gmx.net>
 **************************************************************************/

#pragma once

#include <algorithm>
#include <queue>
#include <vector>

#include "datastructure/fast_reset_flag_array.h"
#include "definitions.h"
#include "utils/randomize.h"

using datastructure::FastResetFlagArray;

namespace partition {
template <bool UseRandomStartHypernode = true>
struct BFSStartNodeSelectionPolicy {
  static inline void calculateStartNodes(std::vector<HypernodeID>& start_nodes, const Configuration& config,
                                         const Hypergraph& hg, const PartitionID k) {
    HypernodeID start_hn = 0;
    if (UseRandomStartHypernode) {
      start_hn = Randomize::instance().getRandomInt(0, hg.initialNumNodes() - 1);
    }
    start_nodes.push_back(start_hn);
    FastResetFlagArray<> in_queue(hg.initialNumNodes());
    FastResetFlagArray<> hyperedge_in_queue(hg.initialNumEdges());

    while (start_nodes.size() != static_cast<size_t>(k)) {
      std::queue<HypernodeID> bfs;
      HypernodeID lastHypernode = -1;
      size_t visited_nodes = 0;
      for (const HypernodeID start_node : start_nodes) {
        bfs.push(start_node);
        in_queue.set(start_node, true);
      }


      while (!bfs.empty()) {
        const HypernodeID hn = bfs.front();
        bfs.pop();
        lastHypernode = hn;
        visited_nodes++;
        for (const HyperedgeID he : hg.incidentEdges(lastHypernode)) {
          if (!hyperedge_in_queue[he]) {
            if (hg.edgeSize(he) <= config.partition.hyperedge_size_threshold) {
              for (const HypernodeID pin : hg.pins(he)) {
                if (!in_queue[pin]) {
                  bfs.push(pin);
                  in_queue.set(pin, true);
                }
              }
            }
            hyperedge_in_queue.set(he, true);
          }
        }
        if (bfs.empty() && visited_nodes != hg.initialNumNodes()) {
          for (const HypernodeID hn : hg.nodes()) {
            if (!in_queue[hn]) {
              bfs.push(hn);
              in_queue.set(hn, true);
            }
          }
        }
      }
      start_nodes.push_back(lastHypernode);
      in_queue.reset();
      hyperedge_in_queue.reset();
    }
  }
};

struct RandomStartNodeSelectionPolicy {
  static inline void calculateStartNodes(std::vector<HypernodeID>& startNodes, const Configuration& UNUSED(config),
                                         const Hypergraph& hg, const PartitionID k) {
    if (k == 2) {
      startNodes.push_back(Randomize::instance().getRandomInt(0, hg.initialNumNodes() - 1));
      return;
    }

    for (PartitionID i = 0; i < k; ++i) {
      while (true) {
        HypernodeID hn = Randomize::instance().getRandomInt(0, hg.initialNumNodes() - 1);
        auto node = std::find(startNodes.begin(), startNodes.end(), hn);
        if (node == startNodes.end()) {
          startNodes.push_back(hn);
          break;
        }
      }
    }
  }
};
}  // namespace partition