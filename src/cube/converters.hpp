#include <cube/exception.hpp>
#include <cube/python.hpp>

#include <vector>

namespace cube {

	template <typename T>
	struct register_vector_converter
	{
		struct to_python
		{
			static PyObject* convert(std::vector<T> const& value)
			{
				boost::python::list list;
				for (auto const& el: value)
					list.append(el);
				return boost::python::incref(list.ptr());
			}
		};

		register_vector_converter()
		{
			boost::python::to_python_converter<
				std::vector<T>,
				to_python
			>();
			boost::python::converter::registry::push_back(
				&register_vector_converter<T>::convertible,
				&register_vector_converter<T>::construct,
				boost::python::type_id<std::vector<T>>()
			);
		}

		static void* convertible(PyObject* ptr)
		{
			// is convertible from list and tuple. if len(ptr) > 0,
			if (PyList_Check(ptr) || PyTuple_Check(ptr))
				return ptr;
			return nullptr;
		}

		static void
		construct(PyObject* ptr,
		          boost::python::converter::rvalue_from_python_stage1_data* data)
		{
			typedef PyObject* (*extractor_t)(PyObject*, ssize_t);

			extractor_t extractor;
			Py_ssize_t len;

			if (PyList_Check(ptr)) {
				extractor = &PyList_GetItem;
				len = PyList_Size(ptr);
			}
			else if (PyTuple_Check(ptr)) {
				extractor = &PyTuple_GetItem;
				len = PyTuple_Size(ptr);
			}
			else
				throw cube::exception::Exception{"convertible has messed up"};

			typedef
				boost::python::converter::rvalue_from_python_storage<std::vector<T>>
				storage_t;

			// Grab pointer to memory into which to construct the new T
			void* storage = (( storage_t* )data)->storage.bytes;

			assert(storage != nullptr && "Boost.Python gave a null storage");

			// in-place construct the new std::vector<T>.
			auto vec = new (storage) std::vector<T>{};

			for (Py_ssize_t i = 0; i < len; ++i) {
				PyObject* item = (*extractor)(ptr, i);
				boost::python::object obj{boost::python::borrowed(item)};
				vec->emplace_back(boost::python::extract<T>(obj));
			}

			// Stash the memory chunk pointer for later use by boost.python
			data->convertible = storage;
		}
	};

}
