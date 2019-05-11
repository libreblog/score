#pragma once
#include <Process/TimeValue.hpp>
#include <Scenario/Document/Event/ExecutionStatus.hpp>
#include <Scenario/Document/VerticalExtent.hpp>
#include <State/Expression.hpp>

#include <score/model/Component.hpp>
#include <score/model/EntityImpl.hpp>
#include <score/model/IdentifiedObject.hpp>
#include <score/model/Identifier.hpp>
#include <score/selection/Selectable.hpp>
#include <score/tools/Metadata.hpp>
#include <score/tools/std/Optional.hpp>

#include <QString>
#include <QVector>

#include <score_plugin_scenario_export.h>
#include <wobjectdefs.h>
class DataStream;
class JSONObject;
class QObject;

namespace Scenario
{
class StateModel;
class ScenarioInterface;
class TimeSyncModel;

class SCORE_PLUGIN_SCENARIO_EXPORT EventModel final
    : public score::Entity<EventModel>
{
  W_OBJECT(EventModel)

  SCORE_SERIALIZE_FRIENDS

public:
  /** Public properties of the class **/
  Selectable selection;

  /** The class **/
  EventModel(
      const Id<EventModel>&,
      const Id<TimeSyncModel>& timesync,
      const VerticalExtent& extent,
      const TimeVal& date,
      QObject* parent);

  template <typename DeserializerVisitor>
  EventModel(DeserializerVisitor&& vis, QObject* parent) : Entity{vis, parent}
  {
    vis.writeTo(*this);
  }

  // Timenode
  void changeTimeSync(const Id<TimeSyncModel>& elt) { m_timeSync = elt; }
  const auto& timeSync() const { return m_timeSync; }

  // States
  void addState(const Id<StateModel>& ds);
  void removeState(const Id<StateModel>& ds);
  void clearStates();
  using StateIdVec = ossia::small_vector<Id<StateModel>, 2>;
  const StateIdVec& states() const;

  // Other properties
  const State::Expression& condition() const;
  OffsetBehavior offsetBehavior() const;
  VerticalExtent extent() const;
  const TimeVal& date() const;
  void translate(const TimeVal& deltaTime);
  ExecutionStatus status() const;

  void setCondition(const State::Expression& arg);
  void setExtent(const Scenario::VerticalExtent& extent);
  void setDate(const TimeVal& date);
  void setStatus(
      Scenario::ExecutionStatus status,
      const Scenario::ScenarioInterface&);
  void setOffsetBehavior(Scenario::OffsetBehavior);

public:
  void extentChanged(const VerticalExtent& arg_1)
      E_SIGNAL(SCORE_PLUGIN_SCENARIO_EXPORT, extentChanged, arg_1)
  void dateChanged(const TimeVal& arg_1)
      E_SIGNAL(SCORE_PLUGIN_SCENARIO_EXPORT, dateChanged, arg_1)
  void conditionChanged(const State::Expression& arg_1)
      E_SIGNAL(SCORE_PLUGIN_SCENARIO_EXPORT, conditionChanged, arg_1)
  void statesChanged() E_SIGNAL(SCORE_PLUGIN_SCENARIO_EXPORT, statesChanged)
  void statusChanged(Scenario::ExecutionStatus status)
      E_SIGNAL(SCORE_PLUGIN_SCENARIO_EXPORT, statusChanged, status)
  void offsetBehaviorChanged(OffsetBehavior arg_1)
      E_SIGNAL(SCORE_PLUGIN_SCENARIO_EXPORT, offsetBehaviorChanged, arg_1)

private:
  Id<TimeSyncModel> m_timeSync;

  StateIdVec m_states;

  State::Expression m_condition;

  VerticalExtent m_extent;
  TimeVal m_date{TimeVal::zero()};

  ExecutionStatusProperty m_status{};
  OffsetBehavior m_offset{};

  W_PROPERTY(
      Scenario::OffsetBehavior,
      offsetBehavior READ offsetBehavior WRITE setOffsetBehavior NOTIFY
          offsetBehaviorChanged,
      W_Final)
};
}

DEFAULT_MODEL_METADATA(Scenario::EventModel, "Event")
TR_TEXT_METADATA(, Scenario::EventModel, PrettyName_k, "Event")
Q_DECLARE_METATYPE(Id<Scenario::EventModel>)
W_REGISTER_ARGTYPE(Id<Scenario::EventModel>)
W_REGISTER_ARGTYPE(Scenario::EventModel)
W_REGISTER_ARGTYPE(const Scenario::EventModel&)
W_REGISTER_ARGTYPE(Scenario::EventModel&)
