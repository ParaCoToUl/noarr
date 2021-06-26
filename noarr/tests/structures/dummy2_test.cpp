#include <catch2/catch.hpp>
//#include "noarr/structures.hpp"

#include <iostream>
#include <array>

#include "noarr/structures/structs.hpp"
#include "noarr/structures/funcs.hpp"
#include "noarr/structures/io.hpp"
#include "noarr/structures/struct_traits.hpp"
#include "noarr/structures/wrapper.hpp"

#include <tuple>

//#include "funcs.hpp"

TEST_CASE("Image", "[image]") {
	//noarr::array<'x', 1920, noarr::array<'y', 1080, noarr::tuple<'p', noarr::scalar<float>, noarr::scalar<float>, noarr::scalar<float>, noarr::scalar<float>>>> image;

	noarr::array<'x', 1920, noarr::array<'y', 1080, noarr::array<'p', 4, noarr::scalar<float>>>> g;
	auto grayscale = noarr::wrap(g);
	std::vector<char> my_blob_buffer(grayscale.get_size());
	char* my_blob = (char*)my_blob_buffer.data();

	SECTION("check is_cube") {
		REQUIRE(noarr::is_cube<decltype(grayscale)>::value);
		//REQUIRE(!noarr::is_cube<decltype(image)>::value);
	}

	
	SECTION("check TODO") {
		//auto value_ref = image | fix<'x'>(0) | fix<'y'>(0) | fix<'p'>(2);
		//std::declval

		//REQUIRE((typeid(image | noarr::fix<'x'>(0) | noarr::fix<'y'>(0) | noarr::fix<'p'>(2)).name()) == "float");
		//auto value_ref = image | noarr::fix<'x'>(0) | noarr::fix<'y'>(0) | noarr::fix<'p'>(2);
		//std::size_t offset = image | noarr::fix<'x', 'y', 'p'>(0, 0, 2) | noarr::offset();
		std::size_t offset = grayscale.fix<'x'>(0).fix<'y'>(0).fix<'p'>(2).offset(); // FIXME: this can be rewritten into `grayscale | offset<'x', 'y', 'z'>()`
		float& value_ref = *((float*)(my_blob + offset));
		value_ref = 0;
		//float& value_ref = image | fix<'x'>(0) | fix<'y'>(0) | fix<'p'>(2) | offset();
	}
}

TEST_CASE("Pipes Vector", "[resizing]")
{
	noarr::vector<'x', noarr::scalar<float>> v;
	auto v2 = v | noarr::set_length<'x'>(10); // transform

	SECTION("size check 1") {
		REQUIRE((v2 | noarr::get_length<'x'>()) == 10);
	}

	auto v3 = v | noarr::set_length<'x'>(20); // transform
	auto v4 = v2 | noarr::set_length<'x'>(30); // transform

	SECTION("size check 2") {
		REQUIRE((v2 | noarr::get_length<'x'>()) == 10);
		REQUIRE((v3 | noarr::get_length<'x'>()) == 20);
		REQUIRE((v4 | noarr::get_length<'x'>()) == 30);
	}

	SECTION("check is_cube 2") {
		REQUIRE(!noarr::is_cube<decltype(v)>::value);
		REQUIRE(noarr::is_cube<decltype(v2)>::value);
		REQUIRE(noarr::is_cube<decltype(v3)>::value);
		REQUIRE(noarr::is_cube<decltype(v4)>::value);
	}

	auto v5 = v4 | noarr::set_length<'x'>(-10); // transform

	SECTION("size check 3") {
		// REQUIRE((v5 | noarr::get_size()) == -10); FIXME: this is absolutely crazy
	}

	SECTION("check is_cube 3") {
		REQUIRE(noarr::is_cube<decltype(v5)>::value);
	}
}



TEST_CASE("Pipes Vector2", "[is_trivial]")
{
	noarr::vector<'x', noarr::scalar<float>> v;
	auto v2 = v | noarr::set_length<'x'>(10); // transform

	SECTION("is_trivial check 1") {
		REQUIRE(std::is_trivial<decltype(v2)>::value);
		REQUIRE(std::is_standard_layout<decltype(v2)>::value);
	}

	auto v3 = v | noarr::set_length<'x'>(20); // transform
	auto v4 = v2 | noarr::set_length<'x'>(30); // transform

	SECTION("is_trivial check 2") {
		REQUIRE(std::is_trivial<decltype(v2)>::value);
		REQUIRE(std::is_standard_layout<decltype(v2)>::value);

		REQUIRE(std::is_trivial<decltype(v3)>::value);
		REQUIRE(std::is_standard_layout<decltype(v3)>::value);

		REQUIRE(std::is_trivial<decltype(v4)>::value);
		REQUIRE(std::is_standard_layout<decltype(v4)>::value);
	}
}


TEST_CASE("Pipes Array", "[is_trivial]")
{
	noarr::array<'x', 1920, noarr::scalar<float>> v;
	auto v2 = v | noarr::set_length<'x'>(10); // transform

	SECTION("is_trivial check 1") {
		REQUIRE(std::is_trivial<decltype(v2)>::value);
		REQUIRE(std::is_standard_layout<decltype(v2)>::value);
	}

	auto v3 = v | noarr::set_length<'x'>(20); // transform
	auto v4 = v2 | noarr::set_length<'x'>(30); // transform

	SECTION("is_trivial check 2") {
		REQUIRE(std::is_trivial<decltype(v2)>::value);
		REQUIRE(std::is_standard_layout<decltype(v2)>::value);

		REQUIRE(std::is_trivial<decltype(v3)>::value);
		REQUIRE(std::is_standard_layout<decltype(v3)>::value);

		REQUIRE(std::is_trivial<decltype(v4)>::value);
		REQUIRE(std::is_standard_layout<decltype(v4)>::value);
	}
}




//////////
// Dots //
//////////




TEST_CASE("Vector", "[resizing]")
{
	noarr::vector<'x', noarr::scalar<float>> v;
	auto w = noarr::wrap(v);
	auto v2 = w.set_length<'x'>(10); // transform

	SECTION("size check 1") {
		REQUIRE((v2.get_length<'x'>()) == 10);
	}

	auto v3 = w.set_length<'x'>(20); // transform
	auto v4 = v2.set_length<'x'>(30); // transform

	SECTION("size check 2") {
		REQUIRE((v2.get_length<'x'>()) == 10);
		REQUIRE((v3.get_length<'x'>()) == 20);
		REQUIRE((v4.get_length<'x'>()) == 30);
	}

	SECTION("check is_cube 2") {
		REQUIRE(!noarr::is_cube<decltype(v)>::value);
		REQUIRE(noarr::is_cube<decltype(v2)>::value);
		REQUIRE(noarr::is_cube<decltype(v3)>::value);
		REQUIRE(noarr::is_cube<decltype(v4)>::value);
	}

	auto v5 = v4.set_length<'x'>(-10); // transform

	SECTION("size check 3") {
		// REQUIRE((v5 | noarr::get_size()) == -10); FIXME: this is absolutely crazy
	}

	SECTION("check is_cube 3") {
		REQUIRE(noarr::is_cube<decltype(v5)>::value);
	}
}



TEST_CASE("Vector2", "[is_trivial]")
{
	noarr::vector<'x', noarr::scalar<float>> v;
	auto w = noarr::wrap(v);
	auto v2 = w.set_length<'x'>(10); // transform

	SECTION("is_trivial check 1") {
		REQUIRE(std::is_trivial<decltype(v2)>::value);
		REQUIRE(std::is_standard_layout<decltype(v2)>::value);
	}

	auto v3 = w.set_length<'x'>(20); // transform
	auto v4 = v2.set_length<'x'>(30); // transform

	SECTION("is_trivial check 2") {
		REQUIRE(std::is_trivial<decltype(v2)>::value);
		REQUIRE(std::is_standard_layout<decltype(v2)>::value);

		REQUIRE(std::is_trivial<decltype(v3)>::value);
		REQUIRE(std::is_standard_layout<decltype(v3)>::value);

		REQUIRE(std::is_trivial<decltype(v4)>::value);
		REQUIRE(std::is_standard_layout<decltype(v4)>::value);
	}
}


TEST_CASE("Array", "[is_trivial]")
{
	noarr::array<'x', 1920, noarr::scalar<float>> v;
	auto w = noarr::wrap(v);
	auto v2 = w.set_length<'x'>(10); // transform

	SECTION("is_trivial check 1") {
		REQUIRE(std::is_trivial<decltype(v2)>::value);
		REQUIRE(std::is_standard_layout<decltype(v2)>::value);
	}

	auto v3 = w.set_length<'x'>(20); // transform
	auto v4 = v2.set_length<'x'>(30); // transform

	SECTION("is_trivial check 2") {
		REQUIRE(std::is_trivial<decltype(v2)>::value);
		REQUIRE(std::is_standard_layout<decltype(v2)>::value);

		REQUIRE(std::is_trivial<decltype(v3)>::value);
		REQUIRE(std::is_standard_layout<decltype(v3)>::value);

		REQUIRE(std::is_trivial<decltype(v4)>::value);
		REQUIRE(std::is_standard_layout<decltype(v4)>::value);
	}
}

template<typename Structure>
struct Bag
{
private:
	noarr::wrapper<Structure> layout_;
	std::unique_ptr<char[]> data_;

public:
	explicit Bag(Structure s)
		: layout_(noarr::wrap(s)),
		data_(std::make_unique<char[]>(layout().get_size())) { }

	constexpr const noarr::wrapper<Structure> &layout() const noexcept { return layout_; }
	
	// noarr::wrapper<Structure> &layout() noexcept { return layout_; } // this version should reallocate the blob (maybe only if it doesn't fit)

	constexpr char *data() const noexcept { return data_.get(); }

	void clear()
	{
		auto size_ = layout().get_size();
		for (std::size_t i = 0; i < size_; i++)
			data_[i] = 0;
	}
};

template<typename Structure>
auto GetBag(Structure s)
{
	return Bag<Structure>(s);
}

template<typename Structure>
auto GetBag(noarr::wrapper<Structure> s)
{
	return Bag<Structure>(s.unwrap());
}

enum class ImageDataLayout { ArrayOfArrays = 1, VectorOfVectors = 2, Zcurve = 3 };

template<ImageDataLayout layout>
struct GetImageStructreStructure;

template<>
struct GetImageStructreStructure<ImageDataLayout::ArrayOfArrays>
{
	static constexpr auto GetImageStructure()
	{
		return noarr::vector<'x', noarr::vector<'y', noarr::scalar<int>>>();
	}
};

template<>
struct GetImageStructreStructure<ImageDataLayout::VectorOfVectors>
{
	static constexpr auto GetImageStructure()
	{
		return noarr::vector<'x', noarr::vector<'y', noarr::scalar<int>>>();
	}
};

template<>
struct GetImageStructreStructure<ImageDataLayout::Zcurve>
{
	static constexpr auto GetImageStructure()
	{
		return noarr::vector<'x', noarr::vector<'y', noarr::scalar<int>>>();
	}
};

template<ImageDataLayout layout, std::size_t width, std::size_t height, std::size_t pixel_range = 256>
void histogram_template_test()
{
	auto image = GetBag(noarr::wrap(GetImageStructreStructure<layout>::GetImageStructure()).template set_length<'x'>(width).template set_length<'y'>(height));
	CHECK(image.layout().get_size() == width * height * sizeof(int));

	int y_size = image.layout().template get_length<'y'>();
	CHECK(y_size == height);

	auto histogram = GetBag(noarr::array<'x', pixel_range, noarr::scalar<int>>());
	CHECK(histogram.layout().get_size() == pixel_range * sizeof(int));

	image.clear();
	histogram.clear();

	int x_size = image.layout().template get_length<'x'>();
	REQUIRE(x_size == width);

	y_size = image.layout().template get_length<'y'>();
	REQUIRE(y_size == height);

	for (int i = 0; i < x_size; i++)
	{
		for (int j = 0; j < y_size; j++)
		{
			//int& pixel_value = *((int*)(image.blob + x_fixed.fix<'y'>(j).offset())); // v1
			//int& pixel_value = *((int*)x_fixed.fix<'y'>(j).get_at(image.blob)); // v2
			int pixel_value = image.layout().template get_at<'x','y'>(image.data(), i, j); // v3

			if (pixel_value != 0)
				FAIL();

			int& histogram_value = histogram.layout().template get_at<'x'>(histogram.data(), pixel_value);
			histogram_value = histogram_value + 1;
		}
	}
}

TEST_CASE("Histogram prototype 720 x 480 with 16 colors", "[Histogram prototype]")
{
	histogram_template_test<ImageDataLayout::ArrayOfArrays, 720, 480, 16>();
}

TEST_CASE("Histogram prototype 720 x 480", "[Histogram prototype]")
{
	histogram_template_test<ImageDataLayout::ArrayOfArrays, 720, 480>();
}

TEST_CASE("Histogram prototype 1080 x 720", "[Histogram prototype]")
{
	histogram_template_test<ImageDataLayout::ArrayOfArrays, 1080, 720>();
}

TEST_CASE("Histogram prototype 1920 x 1080", "[Histogram prototype]")
{
	histogram_template_test<ImageDataLayout::ArrayOfArrays, 1920, 1080>();
}

template<ImageDataLayout layout, std::size_t width, std::size_t height, std::size_t pixel_range = 256>
void histogram_template_test_clear()
{
	auto image = GetBag(noarr::wrap(GetImageStructreStructure<layout>::GetImageStructure()).template set_length<'x'>(width).template set_length<'y'>(height)); // image size 
	auto histogram = GetBag(noarr::array<'x', pixel_range, noarr::scalar<int>>()); // lets say that every image has 256 pixel_range

	image.clear();
	histogram.clear();

	int x_size = image.layout().template get_length<'x'>();
	int y_size = image.layout().template get_length<'y'>();

	for (int i = 0; i < x_size; i++)
	{
		for (int j = 0; j < y_size; j++)
		{
			int pixel_value = image.layout().template get_at<'x', 'y'>(image.data(), i, j);

			int& histogram_value = histogram.layout().template get_at<'x'>(histogram.data(), pixel_value);
			histogram_value = histogram_value + 1;
		}
	}
}
