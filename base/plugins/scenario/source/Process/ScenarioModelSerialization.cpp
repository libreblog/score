#include "Document/Event/EventModel.hpp"
#include "Document/Constraint/ConstraintModel.hpp"
#include "Document/TimeNode/TimeNodeModel.hpp"
#include "Process/Temporal/TemporalScenarioViewModel.hpp"
#include "ScenarioModel.hpp"
#include <iscore/serialization/JSONValueVisitor.hpp>

template<>
void Visitor<Reader<DataStream>>::readFrom(const ScenarioModel& scenario)
{
    m_stream << scenario.m_startEventId;
    m_stream << scenario.m_endEventId;

    // Constraints
    auto constraints = scenario.constraints();
    m_stream << (int) constraints.size();

    for(auto constraint : constraints)
    {
        readFrom(*constraint);
    }

    // Events
    auto events = scenario.events();
    m_stream << (int) events.size();

    for(auto event : events)
    {
        readFrom(*event);
    }

    auto timenodes = scenario.timeNodes();
    m_stream << (int) timenodes.size();

    for(auto timenode : timenodes)
    {
        readFrom(*timenode);
    }

    insertDelimiter();
}

template<>
void Visitor<Writer<DataStream>>::writeTo(ScenarioModel& scenario)
{
    m_stream >> scenario.m_startEventId;
    m_stream >> scenario.m_endEventId;

    // Constraints
    int constraint_count;
    m_stream >> constraint_count;

    for(; constraint_count -- > 0;)
    {
        auto constraint = new ConstraintModel {*this, &scenario};
        scenario.addConstraint(constraint);
    }

    // Events
    int event_count;
    m_stream >> event_count;

    for(; event_count -- > 0;)
    {
        auto evmodel = new EventModel {*this, &scenario};
        scenario.addEvent(evmodel);
    }

    int timenode_count;
    m_stream >> timenode_count;

    for(; timenode_count -- > 0;)
    {
        auto tnmodel = new TimeNodeModel {*this, &scenario};
        scenario.m_timeNodes.push_back(tnmodel);
    }

    checkDelimiter();
}




template<>
void Visitor<Reader<JSONObject>>::readFrom(const ScenarioModel& scenario)
{
    m_obj["StartEventId"] = toJsonValue(scenario.m_startEventId);
    m_obj["EndEventId"] = toJsonValue(scenario.m_endEventId);

    m_obj["Constraints"] = toJsonArray(scenario.constraints());
    m_obj["Events"] = toJsonArray(scenario.events());
    m_obj["TimeNodes"] = toJsonArray(scenario.timeNodes());
}

template<>
void Visitor<Writer<JSONObject>>::writeTo(ScenarioModel& scenario)
{
    scenario.m_startEventId = fromJsonValue<id_type<EventModel>> (m_obj["StartEventId"]);
    scenario.m_endEventId = fromJsonValue<id_type<EventModel>> (m_obj["EndEventId"]);

    for(auto json_vref : m_obj["Constraints"].toArray())
    {
        auto constraint = new ConstraintModel {Deserializer<JSONObject>{json_vref.toObject() },
                          &scenario
    };
        scenario.addConstraint(constraint);
    }

    for(auto json_vref : m_obj["Events"].toArray())
    {
        auto evmodel = new EventModel {
                       Deserializer<JSONObject>{json_vref.toObject() },
                       &scenario};

        scenario.addEvent(evmodel);
    }

    for(auto json_vref : m_obj["TimeNodes"].toArray())
    {
        auto tnmodel = new TimeNodeModel {
                       Deserializer<JSONObject>{json_vref.toObject() },
                       &scenario};

        scenario.m_timeNodes.push_back(tnmodel);
    }
}



void ScenarioModel::serialize(const VisitorVariant& vis) const
{
    switch(vis.identifier)
    {
        case DataStream::type():
        {
            static_cast<DataStream::Serializer&>(vis.visitor).readFrom(*this);
            return;
        }
        case JSONObject::type():
        {
            static_cast<JSONObject::Serializer&>(vis.visitor).readFrom(*this);
            return;
        }
    }

    throw std::runtime_error("ScenarioProcessModel only supports DataStream & JSON serialization");
}

#include "ScenarioFactory.hpp"
ProcessSharedModelInterface* ScenarioFactory::makeModel(
        const VisitorVariant& vis,
        QObject* parent)
{

    switch(vis.identifier)
    {
        case DataStream::type():
        {
            return new ScenarioModel {static_cast<DataStream::Deserializer&>(vis.visitor), parent};
        }
        case JSONObject::type():
        {
            return new ScenarioModel {static_cast<JSONObject::Deserializer&>(vis.visitor), parent};
        }
    }

    throw std::runtime_error("ScenarioProcessModel only supports DataStream & JSON serialization");
}

ProcessViewModelInterface* ScenarioModel::makeViewModel(
        const VisitorVariant& vis,
        QObject* parent)
{
    if(vis.identifier == DataStream::type())
    {
        auto scen = new TemporalScenarioViewModel(
                        static_cast<DataStream::Deserializer&>(vis.visitor),
                        this,
                        parent);
        makeViewModel_impl(scen);
        return scen;
    }
    else if(vis.identifier == JSONObject::type())
    {
        auto scen = new TemporalScenarioViewModel(
                        static_cast<JSONObject::Deserializer&>(vis.visitor),
                        this,
                        parent);
        makeViewModel_impl(scen);
        return scen;
    }

    throw std::runtime_error("ScenarioViewModels only supports DataStream & JSON serialization");
}
