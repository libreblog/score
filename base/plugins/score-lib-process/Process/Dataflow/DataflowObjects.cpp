#include "DataflowObjects.hpp"
#include <Process/Dataflow/DataflowProcess.hpp>
#include <ossia/dataflow/graph_node.hpp>
#include <score/model/path/PathSerialization.hpp>
#include <score/serialization/JSONValueVisitor.hpp>
/*
namespace Dataflow
{
ProcessComponent::ProcessComponent(
    Process::ProcessModel& process,
    DocumentPlugin& doc,
    const Id<score::Component>& id,
    const QString& name,
    QObject* parent):
  Process::GenericProcessComponent<DocumentPlugin>{process, doc, id, name, parent}
{

}
ProcessComponent::~ProcessComponent()
{
}

}
*/
namespace Process
{

Cable::~Cable()
{

}

Cable::Cable(Id<Cable> c): IdentifiedObject{c, "Cable", nullptr} { }

Cable::Cable(const score::DocumentContext& ctx, Id<Cable> c, const CableData& data):
  IdentifiedObject{c, "Cable", nullptr}
{
  m_type = data.type;
  m_source = data.source.try_find(ctx);
  m_sink = data.sink.try_find(ctx);
  m_outlet = data.outlet;
  m_inlet = data.inlet;
}

void Cable::update(const score::DocumentContext& ctx, const CableData& data)
{
  source_node.reset();
  sink_node.reset();
  exec.reset();

  setType(data.type);
  setSource(data.source.try_find(ctx));
  setSink(data.sink.try_find(ctx));
  setOutlet(data.outlet);
  setInlet(data.inlet);
}

CableData Cable::toCableData() const
{
  CableData c;
  c.type = m_type;
  if(m_source) c.source = *m_source;
  if(m_sink) c.sink = *m_sink;
  c.outlet = m_outlet;
  c.inlet = m_inlet;

  return c;
}

CableType Cable::type() const
{
  return m_type;
}

Process::Port* Cable::source() const
{
  return m_source;
}

Process::Port* Cable::sink() const
{
  return m_sink;
}

ossia::optional<int> Cable::outlet() const
{
  return m_outlet;
}

ossia::optional<int> Cable::inlet() const
{
  return m_inlet;
}

void Cable::setType(CableType type)
{
  if (m_type == type)
    return;

  m_type = type;
  emit typeChanged(m_type);
}

void Cable::setSource(Process::Port* source)
{
  if (m_source == source)
    return;
  if(m_source)
  {
    QObject::disconnect(m_srcDeath);
    m_source->removeCable(id());
  }

  m_source = source;

  if(m_source)
  {
    m_srcDeath = connect(m_source, &QObject::destroyed, this, [=] {
      setSource(nullptr);
    });
    m_source->addCable(id());
  }

  emit sourceChanged(m_source);
}

void Cable::setSink(Process::Port* sink)
{
  if (m_sink == sink)
    return;
  if(m_sink)
  {
    QObject::disconnect(m_sinkDeath);
    m_sink->removeCable(id());
  }

  m_sink = sink;

  if(m_sink)
  {
    m_sinkDeath = connect(m_sink, &QObject::destroyed, this, [=] {
      setSink(nullptr);
    });

    m_sink->addCable(id());
  }

  emit sinkChanged(m_sink);
}

void Cable::setOutlet(ossia::optional<int> outlet)
{
  if (m_outlet == outlet)
    return;

  m_outlet = outlet;
  emit outletChanged(m_outlet);
}

void Cable::setInlet(ossia::optional<int> inlet)
{
  if (m_inlet == inlet)
    return;

  m_inlet = inlet;
  emit inletChanged(m_inlet);
}

Port::~Port()
{

}

Port::Port(Id<Port> c, QObject* parent)
  : IdentifiedObject<Port>{c, QStringLiteral("Port"), parent}
{

}

Port::Port(Id<Port> c, const Port& other, QObject* parent)
  : IdentifiedObject<Port>{c, QStringLiteral("Port"), parent}
{
  type = other.type;
  propagate = other.propagate;
  m_cables = other.m_cables;
  m_customData = other.m_customData;
  m_address = other.m_address;
}

Port* Port::clone(QObject* parent) const
{
  return new Port{id(), *this, parent};
}

/*
Node::Node(Id<Node> c, QObject* parent)
  : Entity{std::move(c), QStringLiteral("Node"), parent}
{

}

Node::Node(Id<Node> c, QString name, QObject* parent)
  : Entity{std::move(c), std::move(name), parent}
{

}

Node::~Node()
{
  if(exec)
    exec->clear();
  delete ui;
}
*/
}


template<>
SCORE_LIB_PROCESS_EXPORT void DataStreamReader::read<Process::Port>(const Process::Port& p)
{
  m_stream << p.type << p.propagate << p.m_customData << p.m_address << p.m_cables;
  insertDelimiter();
}
template<>
SCORE_LIB_PROCESS_EXPORT void DataStreamWriter::write<Process::Port>(Process::Port& p)
{
  m_stream >> p.type >> p.propagate >> p.m_customData >> p.m_address >> p.m_cables;
  checkDelimiter();
}

template<>
SCORE_LIB_PROCESS_EXPORT void JSONObjectReader::read<Process::Port>(const Process::Port& p)
{
  obj["Type"] = (int)p.type;
  obj["Propagate"] = p.propagate;
  obj["Custom"] = p.m_customData;
  obj["Address"] = toJsonObject(p.m_address);
  obj["Cables"] = toJsonValueArray(p.m_cables);
}
template<>
SCORE_LIB_PROCESS_EXPORT void JSONObjectWriter::write<Process::Port>(Process::Port& p)
{
  p.type = (Process::PortType)obj["Type"].toInt();
  p.propagate = obj["Propagate"].toBool();
  p.m_customData = obj["Custom"].toString();
  p.m_address = fromJsonObject<State::AddressAccessor>(obj["Address"]);
  fromJsonValueArray(obj["Cables"].toArray(), p.m_cables);
}

template<>
SCORE_LIB_PROCESS_EXPORT void DataStreamReader::read<Process::CableData>(const Process::CableData& p)
{
  m_stream << p.type << p.source << p.sink << p.outlet << p.inlet;
  insertDelimiter();
}
template<>
SCORE_LIB_PROCESS_EXPORT void DataStreamWriter::write<Process::CableData>(Process::CableData& p)
{
  m_stream >> p.type >> p.source >> p.sink >> p.outlet >> p.inlet;
  checkDelimiter();
}

template<>
SCORE_LIB_PROCESS_EXPORT void JSONObjectReader::read<Process::CableData>(const Process::CableData& p)
{
  obj["Type"] = (int)p.type;
  obj["Source"] = toJsonObject(p.source);
  obj["Sink"] = toJsonObject(p.sink);
  obj["Outlet"] = toJsonValue(p.outlet);
  obj["Inlet"] = toJsonValue(p.inlet);
}
template<>
SCORE_LIB_PROCESS_EXPORT void JSONObjectWriter::write<Process::CableData>(Process::CableData& p)
{
  p.type = (Process::CableType) obj["Type"].toInt();
  p.source = fromJsonObject<Path<Process::Port>>(obj["Source"]);
  p.sink = fromJsonObject<Path<Process::Port>>(obj["Sink"]);
  p.outlet = fromJsonValue<optional<int>>(obj["Oulet"]);
  p.inlet = fromJsonValue<optional<int>>(obj["Inlet"]);
}

template<>
SCORE_LIB_PROCESS_EXPORT void DataStreamReader::read<Process::Cable>(const Process::Cable& p)
{
  m_stream << (const Process::CableData&)p;
}
template<>
SCORE_LIB_PROCESS_EXPORT void DataStreamWriter::write<Process::Cable>(Process::Cable& p)
{
  m_stream >> (Process::CableData&)p;
}
template<>
SCORE_LIB_PROCESS_EXPORT void JSONObjectReader::read<Process::Cable>(const Process::Cable& p)
{
  read((const Process::CableData&)p);
}
template<>
SCORE_LIB_PROCESS_EXPORT void JSONObjectWriter::write<Process::Cable>(Process::Cable& p)
{
  write((Process::CableData&)p);
}
