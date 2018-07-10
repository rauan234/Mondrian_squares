#include <iostream>
#include <conio.h>
#include <string>

using namespace std;



#define FieldSize 20 // min - 3, max - 63
#define MaximalPickSize 6

#define Frame 'H'
#define Block 'O'
#define HorisontalBond "---"
#define VerticalBond '|'



std::string Repeat(std::string val, short num) {
	string out;

	for (short q = 0; q < num; q++) {
		out += val;
	}
	
	return out;
}
int min(int a, int b) {
	if (a > b) {
		return b;
	}
	else {
		return a;
	}
}
int max(int a, int b) {
	if (a > b) {
		return a;
	}
	else {
		return b;
	}
}



class Rectangle {
public:
	int8_t x0, y0;
	int8_t x1, y1;
	
	Rectangle(int8_t nx0, int8_t ny0, int8_t nx1, int8_t ny1){
		x0 = nx0;
		y0 = ny0;
		x1 = nx1;
		y1 = ny1;
	}
	Rectangle() {}
	void operator=(Rectangle other) {
		x0 = other.x0;
		y0 = other.y0;
		x1 = other.x1;
		y1 = other.y1;
	}
	void config(int8_t nx0, int8_t ny0, int8_t nx1, int8_t ny1) {
		x0 = nx0;
		y0 = ny0;
		x1 = nx1;
		y1 = ny1;
	}
	short surface() {
		return (x1 - x0) * (y1 - y0);
	}
	std::string str() {
		string out;

		out += '(';
		out += to_string(x0);
		out += ", ";
		out += to_string(y0);
		out += ')';

		out += " x ";

		out += '(';
		out += to_string(x1);
		out += ", ";
		out += to_string(y1);
		out += ')';

		return out;
	}
};

template <typename datatype>
class List {
private:
	struct Element {
		void* payload;
		Element* pointer;
	};

	Element* start = new Element;
	Element* appendix;

public:
	short length = 0;

	List(){
		(*start).pointer = start;
	}
	List<datatype> copy() {
		List<datatype> out;
		
		Element* last = start;
		Element* other = out.start;
		Element* appendix;
		(*out.start).payload = (*start).payload;
		(*out.start).pointer = out.start;
		for(int8_t q = 0; q < length - 1; q++) {
			last = (*last).pointer;

			appendix = new Element;
			(*appendix).payload = &(*static_cast<datatype*>((*last).payload));
			(*appendix).pointer = appendix;
			(*other).pointer = appendix;
			other = appendix;
		}

		out.length = length;

		return out;
	}
	void add(datatype dlt) {
		datatype* pnt = new datatype;
		(*pnt) = dlt;

		if (length == 0) {
			(*start).payload = pnt;
		}
		else {
			appendix = new Element;
			(*appendix).pointer = start;
			(*appendix).payload = pnt;
			start = appendix;
		}

		length += 1;
	}
	void del(short ind) {
		if (ind == 0) {
			start = (*start).pointer;
		}
		else {
			Element* last = start;

			for (short q = 0; q < ind - 1; q++) {
				last = (*last).pointer;
			}

			length -= 1;
			(*last).pointer = (*(*last).pointer).pointer;
		}
	}
	void clear() {
		Element* delpnt;
		for (int8_t q = 0; q < length; q++) {
			delpnt = start;
			start = (*start).pointer;
			delete delpnt;
		}

		start = new Element;
		(*start).pointer = start;
		length = 0;
	}
	datatype pop() {
		if (length == 0) {
			throw std::invalid_argument("Length < 1");
		}

		datatype out = *static_cast<datatype*>((*start).payload);
		start = (*start).pointer;
		length -= 1;
		return out;
	}
	datatype* get(short ind) {
		Element* last = start;

		for (short q = 0; q < ind; q++) {
			last = (*last).pointer;
		}

		return static_cast<datatype*>((*last).payload);
	}
};

template<int8_t fieldsize>
class Field {
private:
	unsigned char tiles[3 * fieldsize * fieldsize / 8 + 1]; // the squear itself
	unsigned char left[((fieldsize + 1) * fieldsize / 2) / 8 + 1]; // this var is made to program couldnt use one type of rectangle twise.
	int sum, empty; // this 2 vars are used for picking optimisation: if empty > sum, squear is unsolvable.

	void write(int8_t xcor, int8_t ycor, int8_t direction, bool val) {
		short ind = (ycor * size + xcor) * 3 + direction;

		tiles[ind / 8] = (tiles[ind / 8] & (255 - int8_t(pow(2, ind % 8))));
		if (val) {
			tiles[ind / 8] = (tiles[ind / 8] | (int8_t(pow(2, ind % 8))));
		}
	}

public:
	int8_t size = fieldsize;
	unsigned short maximal, minimal; // maximal and minimal rectangle surface. Used to find points.

	Field() {
		maximal = 0;
		minimal = INT16_MAX;

		for (short q = 0; q < 3 * fieldsize * fieldsize / 8 + 1; q++) {
			tiles[q] = 0;
		}
		for (short q = 0; q < ((fieldsize + 1) * fieldsize / 2) / 8 + 1; q++) {
			left[q] = 255;
		}

		empty = size * size;
		sum = 0;
		for (int8_t ys = 1; ys <= size; ys++) {
			for (int8_t xs = 1; xs <= ys; xs++) {
				sum += xs * ys;
			}
		}
		sum -= fieldsize * fieldsize;
	}
	Field<fieldsize> copy() {
		Field<fieldsize> out;

		out.size = fieldsize;
		out.maximal = maximal;
		out.minimal = minimal;
		out.sum = sum;
		out.empty = empty;

		for (short q = 0; q < 3 * fieldsize * fieldsize / 8 + 1; q++) {
			out.tiles[q] = tiles[q];
		}
		for (short q = 0; q < ((fieldsize + 1) * fieldsize / 2) / 8 + 1; q++) {
			out.left[q] = left[q];
		}

		return out;
	}
	void operator=(Field other) {
		maximal = other.maximal;
		minimal = other.minimal;

		for (short q = 0; q < 3 * fieldsize * fieldsize / 8 + 1; q++) {
			tiles[q] = other.tiles[q];
		}
		for (short q = 0; q < ((fieldsize + 1) * fieldsize / 2) / 8 + 1; q++) {
			left[q] = other.left[q];
		}
	}
	void rect(int8_t x0, int8_t y0, int8_t x1, int8_t y1) {
		short surface = (x1 - x0) * (y1 - y0);
		if (surface > maximal) {
			maximal = surface;
		}
		if (surface < minimal) {
			minimal = surface;
		}
		empty -= (y1 - y0) * (x1 - x0);

		for (int8_t xcor = x0; xcor < x1; xcor++) {
			for (int8_t ycor = y0; ycor < y1; ycor++) {
				for (int8_t direction = 0; direction < 3; direction++) {

					switch (direction) {
					case 0:
						write(xcor, ycor, 0, 1);
						break;
					case 1:
						if (xcor < x1 - 1) {
							write(xcor, ycor, 1, 1);
						}
						break;
					case 2:
						if (ycor < y1 - 1) {
							write(xcor, ycor, 2, 1);
						}
						break;
					}
				}
			}
		}

		int8_t xs, ys;
		xs = x1 - x0;
		ys = y1 - y0;
		if (xs > ys) {
			swap(xs, ys);
		}
		short ind = ((ys - 1) * ys / 2 + (xs - 1));
		left[ind / 8] = (left[ind / 8] & (255 - int8_t(pow(2, ind % 8))));
	}
	void pass(int8_t xs, int8_t ys) {
		sum -= xs * ys;
	}
	void build(List<Rectangle> lst) {
		for (short q = 0; q < lst.length; q++) {
			rect(
				(*lst.get(q)).x0,
				(*lst.get(q)).y0,
				(*lst.get(q)).x1,
				(*lst.get(q)).y1
			);
		}
	}
	void clear() {
		maximal = 0;
		minimal = INT16_MAX;

		for (short q = 0; q < 3 * fieldsize * fieldsize / 8 + 1; q++) {
			tiles[q] = 0;
		}
		for (short q = 0; q < ((fieldsize + 1) * fieldsize / 2) / 8 + 1; q++) {
			left[q] = 255;
		}

		empty = size * size;
		sum = 0;
		for (int8_t ys = 1; ys <= size; ys++) {
			for (int8_t xs = 1; xs <= ys; xs++) {
				sum += xs * ys;
			}
		}
		sum -= fieldsize * fieldsize;
	}
	bool check(List<Rectangle> rlist) {
		Field<fieldsize> other = copy();
		Rectangle rct;

		for (int8_t q = 0; q < rlist.length; q++) {
			rct = *rlist.get(q);
			if (!other.possible(
				rct.x0,
				rct.y0,
				rct.x1,
				rct.y1
			)) {
				return 0;
			}
			else {
				other.rect(
					rct.x0,
					rct.y0,
					rct.x1,
					rct.y1
				);
			}
		}

		return 1;
		
	}
	bool possible(int8_t x0, int8_t y0, int8_t x1, int8_t y1) {
		short ind;

		if (empty > sum) {
			return 0;
		}

		int8_t xs, ys;
		xs = x1 - x0;
		ys = y1 - y0;
		if (xs > ys) {
			swap(xs, ys);
		}
		ind = ((ys - 1) * ys / 2 + (xs - 1));
		if (!((left[ind / 8] >> ind % 8) & 1)) {
			return 0;
		}

		for (int8_t ycor = y0; ycor < y1; ycor++) {
			for (int8_t xcor = x0; xcor < x1; xcor++) {
				ind = (ycor * size + xcor) * 3;
				if ((tiles[ind / 8] >> ind % 8) & 1) {
					return 0;
				}
			}
		}

		return 1;
	}
	bool filled() {
		short ind;

		for (int8_t ycor = 0; ycor < size; ycor++) {
			for (int8_t xcor = 0; xcor < size; xcor++) {
				ind = (ycor * size + xcor) * 3;
				if (!((tiles[ind / 8] >> ind % 8) & 1)) {
					return 0;
				}
			}
		}

		return 1;
	}
	short score() {
		return maximal - minimal;
	}
	std::string str() {
		string out = "";
		for (int8_t q = 0; q < size * 2 + 1; q++) {
			out += Frame;
			if (q < size * 2) {
				out += ' ';
			}
		}
		out += '\n';

		short ind;
		for (int8_t ycor = 0; ycor < size; ycor++) {
			out += Frame;
			out += ' ';
			for (int8_t xcor = 0; xcor < size; xcor++) {
				ind = (ycor * size + xcor) * 3;
				if ((tiles[ind / 8] >> ind % 8) & 1) {
					out += Block;
				}
				else {
					out += ' ';
				}

				if (xcor < size - 1) {

					ind = (ycor * size + xcor) * 3 + 1;
					if ((tiles[ind / 8] >> ind % 8) & 1) {
						out += HorisontalBond;
					}
					else {
						out += "   ";
					}
				}
			}
			out += ' ';
			out += Frame;
			out += '\n';

			out += Frame;
			out += ' ';
			if (ycor < size - 1) {

				for (int8_t xcor = 0; xcor < size; xcor++) {
					ind = (ycor * size + xcor) * 3 + 2;
					if ((tiles[ind / 8] >> ind % 8) & 1) {
						out += VerticalBond;
					}
					else {
						out += ' ';
					}
					out += ' ';

					if (xcor < size - 1) {
						out += "  ";
					}
				}
				out += Frame;
				out += '\n';
			}
		}
		for (int8_t q = 0; q < size * 2; q++) {
			out += Frame;
			if (q < size * 2 - 1) {
				out += ' ';
			}
		}
		out += '\n';

		out += (Repeat("-", size * 4 + 1) + '\n');
		out += ("Maximal rectangle surface: " + std::to_string(maximal) + '\n');
		if (filled()) {
			out += ("Minimal rectangle surface: " + std::to_string(minimal) + '\n');
			out += ("Total points: " + std::to_string(maximal - minimal));
		}

		return out;
	}
};

template<int8_t fieldsize>
void Pickpart(Field<fieldsize> source, Field<fieldsize>* out, short* best, int8_t xsize, int8_t ysize) {
	Field<fieldsize> other;
	int8_t xs = xsize;
	int8_t ys = ysize;
	xs -= 1;
	if (xs < 1) {
		xs = fieldsize;
		ys -= 1;

		if (ys < 1) {
			return;
		}
	}

	if (source.filled()) {
		if (source.maximal - source.minimal < *best) {
			*best = source.maximal - source.minimal;
			*out = source;
		}
	}
	else {
		for (int8_t xcor = 0; xcor < (fieldsize - xsize + 1); xcor++) {
			for (int8_t ycor = 0; ycor < (fieldsize - ysize + 1); ycor++) {
				if (source.possible(xcor, ycor, xcor + xsize, ycor + ysize)) {
					other = source;
					other.rect(xcor, ycor, xcor + xsize, ycor + ysize);

					Pickpart(other, out, best, xs, ys);
				}
			}
		}

		other = source;
		other.pass(xs, ys);
		Pickpart(other, out, best, xs, ys);
	}
}
template<int8_t fieldsize>
Field<fieldsize> Pick() {
	Field<fieldsize> field;
	Field<fieldsize> out;
	short best = INT16_MAX;

	Pickpart(field, &out, &best, FieldSize - 1, FieldSize - 1);

	return out;
}

template<int8_t fieldsize>
List<Rectangle> BDPrimary() {
	Field<fieldsize> out, clear;



	bool direction = 1;
	int8_t width;

	int8_t xcor, ycor;
	xcor = 0;
	ycor = 0;

	Rectangle rectangle(0, 0, 0, 0);
	List<Rectangle> rlist;

	short sum = 0;
	short num = 0;
	while (!out.filled()) {
		if ((fieldsize - xcor == 2) | (fieldsize - ycor == 2)) {
			if (out.possible(xcor, ycor, fieldsize, fieldsize)) {
				out.rect(xcor, ycor, fieldsize, fieldsize);
			}
			else {
				(*rlist.get(0)).config(
					(*rlist.get(0)).x0,
					(*rlist.get(0)).y0,
					fieldsize,
					fieldsize
				);
				out.clear();
				out.build(rlist);
			}
		}
		else {
			if (direction) {
				if (num == 0) {
					width = 1;
				}
				else {
					width = round(float(sum) / float(num)) / (fieldsize - ycor);
				}
				while (!out.possible(xcor, ycor, xcor + width, fieldsize)) {
					width += 1;

					if (width + xcor > fieldsize) {
						while (1) {
							(*rlist.get(0)).config(
								(*rlist.get(0)).x0,
								(*rlist.get(0)).y0,
								fieldsize,
								fieldsize
							);

							if (clear.check(rlist)) {
								break;
							}
							else {
								rlist.pop();
							}
						}

						return rlist;
					}
				}

				rectangle.config(xcor, ycor, xcor + width, fieldsize);
				rlist.add(rectangle);

				out.rect(xcor, ycor, xcor + width, fieldsize);
				xcor += width;
				sum += width * (fieldsize - ycor);
				num += 1;
			}
			else {
				if (num == 0) {
					width = 1;
				}
				else {
					width = (sum / num) / (fieldsize - xcor);
				}
				while (!out.possible(xcor, ycor, fieldsize, ycor + width)) {
					width += 1;

					if (width + ycor > fieldsize) {
						while (1) {
							(*rlist.get(0)).config(
								(*rlist.get(0)).x0,
								(*rlist.get(0)).y0,
								fieldsize,
								fieldsize
							);

							if (clear.check(rlist)) {
								break;
							}
							else {
								rlist.pop();
							}
						}
					}
				}

				rectangle.config(xcor, ycor, fieldsize, ycor + width);
				rlist.add(rectangle);

				out.rect(xcor, ycor, fieldsize, ycor + width);
				ycor += width;
				sum += width * (fieldsize - xcor);
				num += 1;
			}
		}

		direction = !direction;
	}

	return rlist;
}
template<int8_t fieldsize>
List<Rectangle> BDCut(List<Rectangle> rlist) {
	List<Rectangle> other, result;
	Field<fieldsize> source, empty;
	Rectangle rect, nrect;
	source.build(rlist);
	result = rlist.copy();
	other = rlist.copy();

	short msurface = 0;
	int8_t ind;
	for (int8_t q = 0; q < rlist.length; q++) {
		rect = other.pop();
		if (rect.surface() > msurface) {
			msurface = rect.surface();
			ind = q;
		}
	}
	rect = *rlist.get(ind);
	short xs = rect.x1 - rect.x0;
	short ys = rect.y1 - rect.y0;
	
	short best = source.maximal - source.minimal;
	for (int8_t ycor = rect.y0 + 1; ycor < rect.y1; ycor++) {
		other = rlist.copy();
		other.del(ind);

		nrect.config(
			rect.x0,
			rect.y0,
			rect.x1,
			ycor
		);
		other.add(nrect);
		nrect.config(
			rect.x0,
			ycor,
			rect.x1,
			rect.y1
		);
		other.add(nrect);

		if (empty.check(other)) {
			empty.build(other);
			if (empty.maximal - empty.minimal <= best) {
				best = empty.maximal - empty.minimal;
				result = other.copy();
			}
			empty.clear();
		}
	}
	for (int8_t xcor = rect.x0 + 1; xcor < rect.x1; xcor++) { // the same, but by the X axis
		other = rlist.copy();
		other.del(ind);

		nrect.config(
			rect.x0,
			rect.y0,
			xcor,
			rect.y1
		);
		other.add(nrect);
		nrect.config(
			xcor,
			rect.y0,
			rect.x1,
			rect.y1
		);
		other.add(nrect);

		if (empty.check(other)) {
			empty.build(other);
			if (empty.maximal - empty.minimal <= best) {
				best = empty.maximal - empty.minimal;
				result = other.copy();
			}
			empty.clear();
		}
	}

	return result;
}
template<int8_t fieldsize>
List<Rectangle> BDEnlarge(List<Rectangle> rlist) {
	List<Rectangle> result, out;
	Field<fieldsize> other;
	Rectangle rectangle;
	for (int8_t q = 0; q < rlist.length; q++) {
		out.add(*rlist.get(q));
	}

	int8_t ybound;
	short best = INT16_MAX;
	for (int8_t q = 0; q < rlist.length; q++) {
		for (int8_t diretion = 0; diretion < 4; diretion++) { // !!!!!!!!!!!!!!!!!!! must be 4
			result.clear();

			switch (diretion) {
			case 0: {
				if (rlist.get(q)->y1 < fieldsize - 1) {
					for (int8_t w = 0; w < rlist.length; w++) {

						rectangle = *rlist.get(w);
						if (q != w) {
							if (rectangle.y1 > rlist.get(q)->y0) {
								if (((rectangle.x0 <= rlist.get(q)->x1) * (rectangle.x0 >= rlist.get(q)->x0)) *
									((rectangle.x1 <= rlist.get(q)->x1) * (rectangle.x1 >= rlist.get(q)->x0))) {
									rectangle.config(
										rectangle.x0,
										rectangle.y0 + 1,
										rectangle.x1,
										min(rectangle.y1 + 1, fieldsize)
									);
								}
							}
						}
						else {
							rectangle.config(
								rectangle.x0,
								rectangle.y0,
								rectangle.x1,
								min(rectangle.y1 + 1, fieldsize)
							);
						}

						result.add(rectangle);
					}

					other.clear();
					if (other.check(result)) {
						other.build(result);
						if (other.score() < best) {
							best = other.score();
							out.clear();
							for (int8_t q = 0; q < result.length; q++) {
								out.add(*result.get(q));
							}
						}
					}
				}
				break;
			}
			case 1: {
				if (rlist.get(q)->y0 > 0) {
					for (int8_t w = 0; w < rlist.length; w++) {

						rectangle = *rlist.get(w);
						if (q != w) {
							if (rectangle.y0 < rlist.get(q)->y1) {
								if (((rectangle.x0 <= rlist.get(q)->x1) * (rectangle.x0 >= rlist.get(q)->x0)) *
									((rectangle.x1 <= rlist.get(q)->x1) * (rectangle.x1 >= rlist.get(q)->x0))) {
									rectangle.config(
										rectangle.x0,
										rectangle.y0 - 1,
										rectangle.x1,
										max(rectangle.y1 - 1, 0)
									);
								}
							}
						}
						else {
							rectangle.config(
								rectangle.x0,
								max(rectangle.y0 - 1, 0),
								rectangle.x1,
								rectangle.y1
							);
						}

						result.add(rectangle);
					}

					other.clear();
					if (other.check(result)) {
						other.build(result);
						if (other.score() < best) {
							best = other.score();
							out.clear();
							for (int8_t q = 0; q < result.length; q++) {
								out.add(*result.get(q));
							}
						}
					}
				}
				break;
			}
			case 2: {
				if (rlist.get(q)->x1 < fieldsize - 1) {
					for (int8_t w = 0; w < rlist.length; w++) {

						rectangle = *rlist.get(w);
						if (q != w) {
							if (rectangle.x1 > rlist.get(q)->y0) {
								if (((rectangle.y0 <= rlist.get(q)->y1) * (rectangle.y0 >= rlist.get(q)->y0)) *
									((rectangle.y1 <= rlist.get(q)->y1) * (rectangle.y1 >= rlist.get(q)->y0))) {
									rectangle.config(
										rectangle.x0 + 1,
										rectangle.y0,
										min(rectangle.x1 + 1, fieldsize),
										rectangle.y1
									);
								}
							}
						}
						else {
							rectangle.config(
								rectangle.x0,
								rectangle.y0,
								min(rectangle.x1 + 1, fieldsize),
								rectangle.y1
							);
						}

						result.add(rectangle);
					}

					other.clear();
					if (other.check(result)) {
						other.build(result);
						if (other.score() < best) {
							best = other.score();
							out.clear();
							for (int8_t q = 0; q < result.length; q++) {
								out.add(*result.get(q));
							}
						}
					}
				}
				break;
			}
			case 3: {
				if (rlist.get(q)->y0 > 0) {
					for (int8_t w = 0; w < rlist.length; w++) {

						rectangle = *rlist.get(w);
						if (q != w) {
							if (rectangle.x1 > rlist.get(q)->y0) {
								if (((rectangle.y0 <= rlist.get(q)->y1) * (rectangle.y0 >= rlist.get(q)->y0)) *
									((rectangle.y1 <= rlist.get(q)->y1) * (rectangle.y1 >= rlist.get(q)->y0))) {
									rectangle.config(
										max(rectangle.x0 - 1, 0),
										rectangle.y0,
										max(rectangle.x1 - 1, 0),
										rectangle.y1
									);
								}
							}
						}
						else {
							rectangle.config(
								rectangle.x0,
								max(rectangle.y0 - 1, 0),
								rectangle.x1,
								rectangle.y1
							);
						}

						result.add(rectangle);
					}

					other.clear();
					if (other.check(result)) {
						other.build(result);
						if (other.score() < best) {
							best = other.score();
							out.clear();
							for (int8_t q = 0; q < result.length; q++) {
								out.add(*result.get(q));
							}
						}
					}
				}
				break;
			}
			}
		}
	}

	return out;
}
template<int8_t fieldsize>
List<Rectangle> BDAdvanced(List<Rectangle> rlist) {
	Field<fieldsize> field, other, old;
	List<Rectangle> out, res;
	field.build(rlist);
	out = rlist;

	short best;
	while (1) {
		best = field.score();

		for (int8_t q = 0; q < 2; q++) {
			switch (q) {
			case 0:
				res = BDCut<fieldsize>(out);
				break;
			case 1:
				res = BDEnlarge<fieldsize>(out);
				break;
			}

			other.clear();
			other.build(res);
			if (other.score() <= best) {
				best = other.score();
				out = res;
			}
		}

		if ((other.maximal == old.maximal) and (other.minimal == old.minimal)) {
			break;
		}
		else {
			old = other;
		}
	}

	return out;
}
template<int8_t fieldsize>
Field<fieldsize> BorderDecrease() {
	Field<fieldsize> out;
	List<Rectangle> rlist;

	rlist = BDPrimary<fieldsize>();

	rlist = BDAdvanced<fieldsize>(rlist);

	out.build(rlist);
	return out;
}



int main() {
	Field<FieldSize> field;

	if (FieldSize < MaximalPickSize) {
		field = Pick<FieldSize>();
	}
	else {
		field = BorderDecrease<FieldSize>();
	}

	cout << field.str() << endl;

	_getch();
	return 0;
}


/*
This algorithm is not very optimal - it gives bad results.
I will try to make a new algorithm later.
*/