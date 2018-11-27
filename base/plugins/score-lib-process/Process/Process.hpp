#pragma once
#include <Process/ExpandMode.hpp>
#include <Process/ProcessFlags.hpp>
#include <Process/TimeValue.hpp>

#include <score/model/Component.hpp>
#include <score/model/EntityImpl.hpp>
#include <score/model/Identifier.hpp>
#include <score/model/ModelMetadata.hpp>
#include <score/plugins/SerializableInterface.hpp>
#include <score/selection/Selectable.hpp>
#include <score/selection/Selection.hpp>
#include <score/serialization/DataStreamVisitor.hpp>
#include <score/serialization/JSONVisitor.hpp>
#include <score/serialization/VisitorCommon.hpp>
#include <score/serialization/VisitorInterface.hpp>
#include <score/tools/Metadata.hpp>

#include <ossia/detail/small_vector.hpp>

#include <QByteArray>
#include <QString>

#include <score_lib_process_export.h>
#include <wobjectdefs.h>

#include <vector>

class ProcessStateDataInterface;

namespace Process
{
class Port;
class Inlet;
class Outlet;
class ProcessModelFactory;
class LayerFactory;
class ProcessModel;
class LayerFactory;
using Inlets = ossia::small_vector<Process::Inlet*, 4>;
using Outlets = ossia::small_vector<Process::Outlet*, 4>;

/**
 * @brief The Process class
 *
 * Interface to implement to make a process.
 */
class SCORE_LIB_PROCESS_EXPORT ProcessModel
    : public score::Entity<ProcessModel>,
      public score::SerializableInterface<ProcessModel>
{
  W_OBJECT(ProcessModel)
  SCORE_SERIALIZE_FRIENDS
public:
  Selectable selection;
  ProcessModel(
      TimeVal duration, const Id<ProcessModel>& id, const QString& name,
      QObject* parent);

  ProcessModel(DataStream::Deserializer& vis, QObject* parent);
  ProcessModel(JSONObject::Deserializer& vis, QObject* parent);

  virtual ~ProcessModel();

  // A user-friendly text to show to the users
  virtual QString prettyName() const noexcept;
  virtual QString prettyShortName() const noexcept = 0;
  virtual QString category() const noexcept = 0;
  virtual QStringList tags() const noexcept = 0;
  virtual ProcessFlags flags() const noexcept = 0;

  //// Features of a process
  /// Duration
  void setParentDuration(ExpandMode mode, const TimeVal& t) noexcept;

  virtual bool contentHasDuration() const noexcept;
  virtual TimeVal contentDuration() const noexcept;

  void setDuration(const TimeVal& other) noexcept;
  const TimeVal& duration() const noexcept;

  /// Execution
  virtual void startExecution();
  virtual void stopExecution();
  virtual void reset();

  /// Selection
  virtual Selection selectableChildren() const noexcept;
  virtual Selection selectedChildren() const noexcept;
  virtual void setSelection(const Selection& s) const noexcept;

  double getSlotHeight() const noexcept;
  void setSlotHeight(double) noexcept;

  const Process::Inlets& inlets() const noexcept
  {
    return m_inlets;
  }
  const Process::Outlets& outlets() const noexcept
  {
    return m_outlets;
  }

  Process::Inlet* inlet(const Id<Process::Port>&) const noexcept;
  Process::Outlet* outlet(const Id<Process::Port>&) const noexcept;

  // FIXME ugh
  QWidget* externalUI{};

  // True if the execution is running.
  void execution(bool arg_1)
      E_SIGNAL(SCORE_LIB_PROCESS_EXPORT, execution, arg_1);
  void durationChanged(const TimeVal& arg_1)
      E_SIGNAL(SCORE_LIB_PROCESS_EXPORT, durationChanged, arg_1);
  void useParentDurationChanged(bool arg_1)
      E_SIGNAL(SCORE_LIB_PROCESS_EXPORT, useParentDurationChanged, arg_1);

  void slotHeightChanged(double arg_1)
      E_SIGNAL(SCORE_LIB_PROCESS_EXPORT, slotHeightChanged, arg_1);
  void prettyNameChanged()
      E_SIGNAL(SCORE_LIB_PROCESS_EXPORT, prettyNameChanged);

  void inletsChanged() E_SIGNAL(SCORE_LIB_PROCESS_EXPORT, inletsChanged);
  void outletsChanged() E_SIGNAL(SCORE_LIB_PROCESS_EXPORT, outletsChanged);

  void controlAdded(const Id<Process::Port>& arg_1)
      E_SIGNAL(SCORE_LIB_PROCESS_EXPORT, controlAdded, arg_1);
  void controlRemoved(const Process::Port& arg_1)
      E_SIGNAL(SCORE_LIB_PROCESS_EXPORT, controlRemoved, arg_1);

  void benchmark(double arg_1)
      E_SIGNAL(SCORE_LIB_PROCESS_EXPORT, benchmark, arg_1);
  void externalUIVisible(bool v) const
      E_SIGNAL(SCORE_LIB_PROCESS_EXPORT, externalUIVisible, v);

protected:

  // Used to scale the process.
  // This should be commutative :
  //   setDurationWithScale(2); setDurationWithScale(3);
  // yields the same result as :
  //   setDurationWithScale(3); setDurationWithScale(2);
  virtual void setDurationAndScale(const TimeVal& newDuration) noexcept;

  // Does nothing if newDuration < currentDuration
  virtual void setDurationAndGrow(const TimeVal& newDuration) noexcept;

  // Does nothing if newDuration > currentDuration
  virtual void setDurationAndShrink(const TimeVal& newDuration) noexcept;

  Process::Inlets m_inlets;
  Process::Outlets m_outlets;

private:
  TimeVal m_duration;
  double m_slotHeight{}; //! Height in full view
};

SCORE_LIB_PROCESS_EXPORT
ProcessModel* parentProcess(QObject* obj) noexcept;
SCORE_LIB_PROCESS_EXPORT
const ProcessModel* parentProcess(const QObject* obj) noexcept;
}
DEFAULT_MODEL_METADATA(Process::ProcessModel, "Process")

Q_DECLARE_METATYPE(Id<Process::ProcessModel>)
W_REGISTER_ARGTYPE(Id<Process::ProcessModel>)
W_REGISTER_ARGTYPE(OptionalId<Process::ProcessModel>)

Q_DECLARE_METATYPE(const Process::ProcessModel*)
W_REGISTER_ARGTYPE(const Process::ProcessModel*)
W_REGISTER_ARGTYPE(QPointer<const Process::ProcessModel>)
W_REGISTER_ARGTYPE(QPointer<Process::ProcessModel>)
