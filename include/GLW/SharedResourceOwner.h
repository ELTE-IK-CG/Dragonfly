#pragma once

#include "common.h"

namespace GLWRAPPER_NS {

	template<typename T>
	class SharedResourceOwner {

		// number of owners except this
		mutable size_t* reference_counter = nullptr;
		// raw resource data
		T data = T();

	protected:

		SharedResourceOwner() {}

	public:

		~SharedResourceOwner() {
			Reset();
		}

		// Copy constructor
		SharedResourceOwner(const SharedResourceOwner& other) noexcept {
			other.IncreaseCounter();

			reference_counter = other.reference_counter;
			data = other.data;
		}

		// Copy operator
		SharedResourceOwner &operator=(const SharedResourceOwner& other) noexcept {

			if (&other != this) {
				Reset();

				other.IncreaseCounter();

				reference_counter = other.reference_counter;
				data = other.data;
			}

			return *this;
		}

		// Move constructor
		SharedResourceOwner(SharedResourceOwner &&other) noexcept
		{
			reference_counter = other.reference_counter;
			data = std::move(other.data);

			other.reference_counter = nullptr;
			other.data = T();
		}

		// Move operator
		SharedResourceOwner &operator=(SharedResourceOwner &&other) noexcept
		{
			Reset();

			reference_counter = other.reference_counter;
			data = std::move(other.data);

			other.reference_counter = nullptr;
			other.data = T();

			return *this;
		}

		// delete resource
		void Reset(T&& new_data = T()) {
			if (reference_counter == nullptr) {
				data.Reset();
			}
			else if (*reference_counter == 0) {
				delete reference_counter;
				data.Reset();
			}
			else {
				(*reference_counter)--;
			}

			reference_counter = nullptr;
			data = std::move(new_data);
		}

		// Get raw data
		const T& Get() const {
			return data;
		}

	private:

		void IncreaseCounter() const {
			if (reference_counter == nullptr) {
				reference_counter = new size_t(0);
			}
			
			reference_counter++;
			
		}

	};

}