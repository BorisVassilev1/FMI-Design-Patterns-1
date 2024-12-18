
#include <algorithm>
#include <cstdio>
#include <sstream>
#include <stdexcept>
#include "src/abstract_factory.hpp"
#include "src/figure_factory.hpp"
#include "src/type_registry.hpp"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include "src/figures.hpp"

TEST_CASE("doctest sanity check") { CHECK(1 == 1); }

TEST_CASE("figure constructors") {
	Triangle  t(3, 4, 5);
	Circle	  c(1);
	Rectangle r(1, 2);
}

TEST_CASE("figure invalid arguments") {
	CHECK_THROWS_AS(Triangle(1, 2, 4), std::invalid_argument);
	CHECK_THROWS_AS(Triangle(1, 2, 0), std::invalid_argument);
	CHECK_THROWS_AS(Triangle(1, 0, 2), std::invalid_argument);
	CHECK_THROWS_AS(Triangle(0, 2, 1), std::invalid_argument);

	CHECK_THROWS_AS(Triangle(1, 2, -1), std::invalid_argument);
	CHECK_THROWS_AS(Triangle(1, -2, 1), std::invalid_argument);
	CHECK_THROWS_AS(Triangle(-1, 2, 1), std::invalid_argument);

	CHECK_THROWS_AS(Circle(0), std::invalid_argument);
	CHECK_THROWS_AS(Circle(-1), std::invalid_argument);

	CHECK_THROWS_AS(Rectangle(1, 0), std::invalid_argument);
	CHECK_THROWS_AS(Rectangle(0, 1), std::invalid_argument);
	CHECK_THROWS_AS(Rectangle(-1, 1), std::invalid_argument);
	CHECK_THROWS_AS(Rectangle(1, -1), std::invalid_argument);
}

TEST_CASE("figure invalid arguments NAN") {
	CHECK_THROWS_AS(Triangle(1, 2, NAN), std::invalid_argument);
	CHECK_THROWS_AS(Triangle(1, NAN, 2), std::invalid_argument);
	CHECK_THROWS_AS(Triangle(NAN, 2, 1), std::invalid_argument);

	CHECK_THROWS_AS(Circle(NAN), std::invalid_argument);

	CHECK_THROWS_AS(Rectangle(1, NAN), std::invalid_argument);
	CHECK_THROWS_AS(Rectangle(NAN, 1), std::invalid_argument);
}

TEST_CASE("figure overflow") {
	float flt_max = std::numeric_limits<float>::max();

	CHECK_THROWS_AS(Triangle(1, 2, flt_max), std::overflow_error);
	CHECK_THROWS_AS(Triangle(1, flt_max, 2), std::overflow_error);
	CHECK_THROWS_AS(Triangle(flt_max, 2, 1), std::overflow_error);
	CHECK_THROWS_AS(Triangle(flt_max, flt_max, flt_max), std::overflow_error);

	CHECK_THROWS_AS(Circle(float(flt_max)), std::overflow_error);
	CHECK_THROWS_AS(Rectangle(flt_max, 1), std::overflow_error);
	CHECK_THROWS_AS(Rectangle(1, flt_max), std::overflow_error);
}

TEST_CASE("perimeters") {
	CHECK_EQ(Triangle(3, 4, 5).perimeter(), 12);
	CHECK_EQ(Rectangle(1, 2).perimeter(), 6);
	CHECK_LT(std::abs(Circle(1).perimeter() - 2 * M_PI), 0.01);
}

TEST_CASE("clone triangle") {
	Figure* f = new Triangle(3, 4, 5);
	Figure* t = (Figure*)f->clone();
	CHECK_EQ(f->perimeter(), t->perimeter());
	delete f;
	delete t;
}

TEST_CASE("clone Circle") {
	Figure* f = new Circle(1);
	Figure* t = (Figure*)f->clone();
	CHECK_EQ(f->perimeter(), t->perimeter());
	delete f;
	delete t;
}

TEST_CASE("clone rectangle") {
	Figure* f = new Rectangle(1, 2);
	Figure* t = (Figure*)f->clone();
	CHECK_EQ(f->perimeter(), t->perimeter());
	delete f;
	delete t;
}

TEST_CASE("stringify figures") {
	CHECK_EQ((std::string)Triangle(3, 4, 5), "Triangle 3 4 5");
	CHECK_EQ((std::string)Circle(1), "Circle 1");
	CHECK_EQ((std::string)Rectangle(1, 2), "Rectangle 1 2");
}

TEST_CASE("inheritance") {
	auto v = TypeRegistry::getChildren().at("Figure");
	CHECK_NE(std::find(v.begin(), v.end(), "Triangle"), v.end());
	CHECK_NE(std::find(v.begin(), v.end(), "Rectangle"), v.end());
	CHECK_NE(std::find(v.begin(), v.end(), "Circle"), v.end());

	v = TypeRegistry::getChildren().at("Cloneable");
	CHECK_NE(std::find(v.begin(), v.end(), "Figure"), v.end());
	CHECK_NE(std::find(v.begin(), v.end(), "Triangle"), v.end());
	CHECK_NE(std::find(v.begin(), v.end(), "Rectangle"), v.end());
	CHECK_NE(std::find(v.begin(), v.end(), "Circle"), v.end());

	v = TypeRegistry::getChildren().at("FigureFactory");
	CHECK_NE(std::find(v.begin(), v.end(), "RandomFigureFactory"), v.end());
	CHECK_NE(std::find(v.begin(), v.end(), "IstreamFigureFactory"), v.end());
	CHECK_NE(std::find(v.begin(), v.end(), "FileFigureFactory"), v.end());
	CHECK_NE(std::find(v.begin(), v.end(), "STDINFigureFactory"), v.end());

	v = TypeRegistry::getParents().at("STDINFigureFactory");
	CHECK_NE(std::find(v.begin(), v.end(), "FigureFactory"), v.end());
	CHECK_NE(std::find(v.begin(), v.end(), "IstreamFigureFactory"), v.end());
}

TEST_CASE("Random Factory") {
	srand(42);
	RandomFigureFactory ff;
	for (int i = 0; i < 100; ++i) {
		auto f = ff.create();
		CHECK(f->perimeter() >= 0);
		if (dynamic_cast<Triangle*>(f.get())) {
			CHECK(f->perimeter() >= 15);
			CHECK(f->perimeter() <= 30);
		}
		if (dynamic_cast<Circle*>(f.get())) {
			CHECK(f->perimeter() >= 31);
			CHECK(f->perimeter() <= 63);
		}
		if (dynamic_cast<Rectangle*>(f.get())) {
			CHECK(f->perimeter() >= 20);
			CHECK(f->perimeter() <= 40);
		}
	}
}

TEST_CASE("Random Factory with figure count") {
	srand(42);
	RandomFigureFactory ff(100);
	for (int i = 0; i < 100; ++i) {
		auto f = ff.create();
		CHECK(f->perimeter() >= 0);
		if (dynamic_cast<Triangle*>(f.get())) {
			CHECK(f->perimeter() >= 15);
			CHECK(f->perimeter() <= 30);
		}
		if (dynamic_cast<Circle*>(f.get())) {
			CHECK(f->perimeter() >= 31);
			CHECK(f->perimeter() <= 63);
		}
		if (dynamic_cast<Rectangle*>(f.get())) {
			CHECK(f->perimeter() >= 20);
			CHECK(f->perimeter() <= 40);
		}
	}
	CHECK_EQ(ff.create(), nullptr);
}

TEST_CASE("Istream Factory") {
	std::stringstream		ss("Triangle 3.0 4.00 5e0\nCircle 1\nRectangle 2 3");
	IstreamFigureFactory	ff(ss);
	std::unique_ptr<Figure> f;

	f = ff.create();
	CHECK_EQ((std::string)*f, "Triangle 3 4 5");

	f = ff.create();
	CHECK_EQ((std::string)*f, "Circle 1");

	f = ff.create();
	CHECK_EQ((std::string)*f, "Rectangle 2 3");
}

TEST_CASE("File Figure Factory") {
	std::ofstream file(PROJECT_SOURCE_DIR "/tmp/test_file.txt");
	file << "Triangle 3 4 5\nCircle 1 \n Rectangle 2 3";
	file.close();

	FileFigureFactory		ff(PROJECT_SOURCE_DIR "/tmp/test_file.txt");
	std::unique_ptr<Figure> f;

	f = ff.create();
	CHECK_EQ((std::string)*f, "Triangle 3 4 5");

	f = ff.create();
	CHECK_EQ((std::string)*f, "Circle 1");

	f = ff.create();
	CHECK_EQ((std::string)*f, "Rectangle 2 3");
}

TEST_CASE("STDIN Figure Factory") {
	std::istringstream is("Triangle 3 4 5\nCircle 1 \n Rectangle 2 3\n askd");
	std::cin.rdbuf(is.rdbuf());
	STDINFigureFactory		ff;
	std::unique_ptr<Figure> f;

	f = ff.create();
	CHECK_EQ((std::string)*f, "Triangle 3 4 5");
	f = ff.create();
	CHECK_EQ((std::string)*f, "Circle 1");
	f = ff.create();
	CHECK_EQ((std::string)*f, "Rectangle 2 3");
}

TEST_CASE("STDIN Figure Factory with figure count") {
	std::istringstream is("Triangle 3 4 5\nCircle 1 \n Rectangle 2 3\n askd");
	std::cin.rdbuf(is.rdbuf());
	STDINFigureFactory		ff(1);
	std::unique_ptr<Figure> f;

	f = ff.create();
	CHECK_EQ((std::string)*f, "Triangle 3 4 5");
	CHECK_EQ(ff.create(), nullptr);
}

TEST_CASE("Istream Factory exceptions") {
	std::unique_ptr<Figure> f = nullptr;
	CHECK_EQ(f = figureFromString(""), nullptr);
	CHECK_THROWS_AS(f = figureFromString("asdj"), std::runtime_error);
	CHECK_THROWS_AS(f = figureFromString("RandomFigureFactory"), std::runtime_error);
	CHECK_THROWS_AS(f = figureFromString("STDINFigureFactory"), std::runtime_error);

	CHECK_THROWS_AS(f = figureFromString("Triangle "), std::runtime_error);
	CHECK_THROWS_AS(f = figureFromString("Triangle 1 "), std::runtime_error);
	CHECK_THROWS_AS(f = figureFromString("Triangle 1 2"), std::runtime_error);

	CHECK_THROWS_AS(f = figureFromString("Circle "), std::runtime_error);

	CHECK_THROWS_AS(f = figureFromString("Rectangle "), std::runtime_error);
	CHECK_THROWS_AS(f = figureFromString("Rectangle 1"), std::runtime_error);

	CHECK_THROWS_AS(f = figureFromString("Triangle 1 2 3"), std::invalid_argument);
}

TEST_CASE("Abstract Factory") {
	std::ifstream file(PROJECT_SOURCE_DIR "/tmp/test_file.txt", std::ios_base::trunc);
	file.close();

	std::istringstream is("Random -1\nSTDIN -1\n File " PROJECT_SOURCE_DIR
						  "/tmp/test_file.txt \n Triangle 1 2 3\n asaskd");
	std::ostringstream os;
	os.setstate(std::ios_base::badbit);

	AbstractFigureFactory aff(is);

	std::unique_ptr<FigureFactory> f;
	f = aff.create();
	CHECK_NE(dynamic_cast<RandomFigureFactory*>(f.get()), nullptr);

	f = aff.create();
	CHECK_NE(dynamic_cast<STDINFigureFactory*>(f.get()), nullptr);

	f = aff.create();
	CHECK_NE(dynamic_cast<FileFigureFactory*>(f.get()), nullptr);

	CHECK_THROWS_AS(aff.create(), std::runtime_error);
}
