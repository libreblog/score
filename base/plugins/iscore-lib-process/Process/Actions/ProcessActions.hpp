#pragma once
#include <iscore/actions/Action.hpp>
namespace Process
{
template<typename T>
struct EnableWhenFocusedProcessIs;
}

#define ISCORE_DECLARE_FOCUSED_PROCESS_CONDITION(Type)                              \
template<>                                                                \
struct Process::EnableWhenFocusedProcessIs<Type> final : public iscore::FocusActionCondition       \
{                                                                                   \
    public:                                                                         \
    static iscore::ActionConditionKey static_key() { return iscore::ActionConditionKey{"FocusedProcessIs" #Type }; }     \
                                                                                    \
    EnableWhenFocusedProcessIs():                                                   \
        iscore::FocusActionCondition{static_key()}                                  \
    {                                                                               \
                                                                                    \
    }                                                                               \
                                                                                    \
                                                                                    \
    private:                                                                        \
        void action(iscore::ActionManager& mgr, iscore::MaybeDocument doc) override \
        {                                                                           \
            if(!doc)                                                                \
            {   qDebug("No doc");                                                                    \
                setEnabled(mgr, false);                                             \
                return;                                                             \
            }                                                                       \
                                                                                    \
            auto obj = doc->focus.get();                                            \
            if(!obj)                                                                \
            {   qDebug("No obj");                                                                  \
                setEnabled(mgr, false);                                             \
                return;                                                             \
            }                                                                       \
                                                                                    \
            auto layer = dynamic_cast<const Process::LayerModel*>(obj);             \
            if(!layer)                                                              \
            {   qDebug("No layer");                                                                    \
                setEnabled(mgr, false);                                             \
                return;                                                             \
            }                                                                       \
            qDebug() << "process type" << layer->processModel().objectName();                                                                         \
            if(dynamic_cast<Type*>(&layer->processModel()))                            \
            {                                                                      \
                setEnabled(mgr, true);                                              \
            } else { qDebug("bad"); setEnabled(mgr, false);   }                                                                   \
        }                                                                           \
};
