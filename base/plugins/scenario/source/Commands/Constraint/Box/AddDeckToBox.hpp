#pragma once
#include <core/presenter/command/SerializableCommand.hpp>
#include <tools/ObjectPath.hpp>

class AddDeckToBoxTest;
class AddProcessViewModelToDeckTest;
namespace Scenario
{
	namespace Command
	{
		/**
		 * @brief The AddDeckToBox class
		 *
		 * Adds an empty deck to a constraint.
		 */
		class AddDeckToBox : public iscore::SerializableCommand
		{
				friend class ::AddDeckToBoxTest;
				friend class ::AddProcessViewModelToDeckTest;
			public:
				AddDeckToBox(ObjectPath&& boxPath);

				virtual void undo() override;
				virtual void redo() override;
				virtual int id() const override;
				virtual bool mergeWith(const QUndoCommand* other) override;

			protected:
				virtual void serializeImpl(QDataStream&) override;
				virtual void deserializeImpl(QDataStream&) override;

			private:
				ObjectPath m_path;

				int m_createdDeckId{};
		};
	}
}
