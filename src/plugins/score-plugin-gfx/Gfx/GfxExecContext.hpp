#pragma once
#include <Process/ExecutionAction.hpp>

#include <Gfx/GfxContext.hpp>

namespace Gfx
{

class GfxExecutionAction final : public Execution::ExecutionAction
{
  SCORE_CONCRETE("06f48270-35a4-44d2-929a-e67b8e2904f5")
public:
  GfxExecutionAction(gfx_window_context& w) : ui{&w}
  {
    prev_edges.container.reserve(100);
    edges.container.reserve(100);
  }
  gfx_window_context* ui{};

  void startTick(const ossia::audio_tick_state& st) override { edges.clear(); }

  void setEdge(port_index source, port_index sink) { edges.insert({source, sink}); }

  void endTick(const ossia::audio_tick_state& st) override
  {
    if (edges != prev_edges)
    {
      {
        std::lock_guard l{ui->edges_lock};
        ui->new_edges.container.assign(edges.container.begin(), edges.container.end());
      }

      prev_edges.container.assign(edges.container.begin(), edges.container.end());
      ui->edges_changed = true;
    }
  }

  ossia::flat_set<std::pair<port_index, port_index>> prev_edges;
  ossia::flat_set<std::pair<port_index, port_index>> edges;
};

}
