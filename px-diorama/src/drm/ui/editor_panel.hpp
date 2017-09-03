// name: editor_panel.hpp
// type: c++
// desc: class
// auth: isource

#pragma once

#include <px/ui/panel.hpp>
#include "drm/environment.hpp"

#include <px/ui/board.hpp>
#include <px/ui/button.hpp>
#include "edit.hpp"

namespace px {
	namespace ui {

		class editor_panel
			: public panel
		{
		public:
			void assign_environment(environment * env) noexcept
			{
				m_environment = env;
			}

		public:
			virtual ~editor_panel()
			{
			}
			editor_panel()
				: m_environment(nullptr)
			{
				make<board>(fill, color{ 0.5, 0.0, 0.0, 0.75 }); // background

				// tag text field
				auto tag_block = make<board>({ { 0.0, 0.0 },{ 1,1 },{ -2,1 },{ 1.0,0.0 } }, color{ 1, 1, 1 });
				edit * text_field = tag_block->make<edit>(fill).get();

				// create button
				auto create_block = make<board>({ { 0.0, 0.0 },{ 1,3 },{ 6,1 },{ 0, 0 } }, color{ 1, 0.5,0, 1 });
				create_block->make<text>(fill, "scheme");
				auto create_mechanics = create_block->make<button>(fill);

				create_mechanics->on_click([this, text_field](int /* mouse_button */) {
					compile_unit(text_field->current());
				});

				// import button
				auto button_block = make<board>({ { 0.0, 0.0 },{ 1,5 },{ 6,1 },{ 0, 0 } }, color{ 1, 0.5,0, 1 });
				button_block->make<text>(fill, "import");
				auto button_mechanics = button_block->make<button>(fill);

				button_mechanics->on_click([this, text_field](int /* mouse_button */) {
					import_unit(text_field->current());
				});
			}

		private:
			void compile_unit(std::string const& name)
			{
				px_assert(m_environment);
				if (m_environment) {
					auto placeable = m_environment->compile_unit(name);
					m_environment->spawn(placeable, target_location());
				}
			}
			void import_unit(std::string const& name)
			{
				px_assert(m_environment);
				if (m_environment) {
					auto placeable = m_environment->import_unit(name);
					m_environment->spawn(placeable, target_location());
				}
			}
			point2 const& target_location() const
			{
				px_assert(m_environment);
				return m_environment->player()->position();
			}

		private:
			environment * m_environment;
		};
	}
}