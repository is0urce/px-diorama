// name: delete_release_block
// type: c++ template class

#pragma once

// polymorphic release block for typed destructor call

#include <px/memory/poly_release_block.hpp>

namespace px {

	template <typename T>
	class delete_release_block final
		: public poly_release_block
	{
	public:
		virtual ~delete_release_block()
		{
		}
		delete_release_block(T * raw)
			: original(raw)
		{
		}

	protected:
		virtual void release_block() noexcept override
		{
			delete original;
		}

	private:
		T * original;
	};

}