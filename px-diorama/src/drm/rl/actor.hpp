// name: actor.hpp
// type: c++ header
// desc: class definition
// auth: is0urce

#pragma once

#include <px/rl/body.hpp>
#include <px/rl/buff.hpp>
#include <px/rl/inventory.hpp>

#include <functional>

namespace px {
	namespace rl {

		template<typename Parent, typename Effect, typename EquipmentSlot, typename Environment = void*>
		class actor
			: public body
			, public inventory<Effect>
		{
		public:
			typedef Effect effect_type;
			typedef Environment environment_type;
			typedef Parent parent_type;
			typedef EquipmentSlot slot_type;

			typedef std::function<void(environment_type&, parent_type&)> buff_event;
			typedef buff<effect_type, buff_event> buff_type;

		public:
			using inventory<effect_type>::add; // use parent method in overloading

		public:
			bool dead() const noexcept
			{
				auto hp = health();
				return hp && hp->empty();
			}

			void add(buff_type buff, environment_type& env)
			{
				m_buffs.push_back(buff);
				buff.apply(env, *static_cast<Parent*>(this));
			}
			void tick(environment_type& env)
			{
				for (auto it = m_buffs.begin(), last = m_buffs.end(); it != last; )
				{
					it->tick(env, *static_cast<Parent*>(this));
					if (it->expired())
					{
						it->expire(env, *static_cast<Parent*>(this));
						it = m_buffs.erase(it);
					}
					else
					{
						++it;
					}
				}
			}
			template <typename T, typename BinaryOperation>
			T accumulate(T start, BinaryOperation fold) const
			{
				for (auto const& buff : m_buffs)
				{
					start = buff.accumulate(start, fold);
				}
				// item buffs
				for (auto const& item : m_equipment)
				{
					if (item)
					{
						start += item->accumulate(start, fold);
					}
				}
				return start;
			}

			template <effect_type TValue>
			enhancement<effect_type> accumulate() const
			{
				enhancement<effect_type> start{};

				// affect buffs
				for (auto const& buff : m_buffs)
				{
					start += buff.accumulate<TValue>();
				}

				// item buffs
				for (auto const& item : m_equipment)
				{
					if (item)
					{
						start += item->accumulate<TValue>();
					}
				}

				return start;
			}

			// equipment

			void clear_equipment()
			{
				for (auto& item : m_equipment)
				{
					item.reset();
				}
			}

			bool is_equipped(item_ptr const& item) const noexcept
			{
				for (auto const& equip : m_equipment)
				{
					if (equip == item) return true;
				}
				return false;
			}
			bool is_equipped(slot_type slot) const noexcept
			{
				return m_equipment[static_cast<size_t>(slot)] != nullptr;
			}
			bool is_equipped(item_ptr const& item, slot_type slot) const noexcept
			{
				return m_equipment[static_cast<size_t>(slot)] == item;
			}

			void equip(item_ptr item, slot_type slot)
			{
				m_equipment[static_cast<size_t>(slot)] = item;
			}
			void unequip(slot_type slot)
			{
				m_equipment[static_cast<size_t>(slot)].reset();
			}

			// returns true if item equipped
			bool equip(item_ptr item)
			{
				auto pair = item->find<rl::effect::equipment>();
				bool success = std::get<0>(pair);
				if (success)
				{
					equip(item, static_cast<slot_type>(std::get<1>(pair).subtype));
				}
				return success;
			}
			// returns true if item is unequipped
			bool unequip(item_ptr item)
			{
				for (auto it = m_equipment.begin(), last = m_equipment.end(); it != last; ++it)
				{
					if (*it == item)
					{
						*it = nullptr;
						return true;
					}
				}
				return false;
			}

			template <slot_type Slot>
			bool is_equipped_slot() const noexcept
			{
				return m_equipment[static_cast<size_t>(Slot)] != null;
			}
			template <slot_type Slot>
			item_ptr const& equipment()
			{
				return m_equipment[static_cast<size_t>(Slot)];
			}
			template <slot_type Slot>
			void equip_slot(item_ptr item)
			{
				m_equipment[static_cast<size_t>(Slot)] = item;
			}
			template <slot_type Slot>
			void unequip_slot()
			{
				m_equipment[static_cast<size_t>(Slot)].reset();
			}

		public:
			actor()
				: m_equipment(static_cast<size_t>(slot_type::max_value) + 1)
			{
			}
			virtual ~actor()
			{
			}

		private:
			std::vector<item_ptr> m_equipment;
			std::list<buff_type> m_buffs;
		};
	}
}