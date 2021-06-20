#include "stone/Basic/PipelineEngine.h"

using namespace stone;

void PipelineEngine::Add(const Pipeline *pipeline) {
  assert(pipeline && "Null pipeline");

  // entries.Add(
  //    std::make_pair<PipelineType, Pipeline>(pipeline->GetType(), pipeline));
}

Pipeline *PipelineEngine::Get(PipelineType ty) {
  // for (auto p : entries) {
  //   if (p.first == ty) {
  //     return p.second;
  //   }
  // }
  return nullptr;
}
