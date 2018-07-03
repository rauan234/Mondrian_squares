#include <iostream>
#include <conio.h>
#include <string>

using namespace std;



#define FieldSize 5

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



template<int8_t fieldsize>
class Field {
private:
	int8_t tiles[3 * fieldsize * fieldsize / 8 + 1]; // the sqyear itself
	int8_t left[((fieldsize + 1) * fieldsize / 2) / 8 + 1]; // this var is made to program couldnt use one type of rectangle twise.
	short sum, empty; // this 2 vars are used for picking optimisation: if empty > sum, squear is unsolvable.

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

		for (int8_t q = 0; q < 3 * fieldsize * fieldsize / 8 + 1; q++) {
			tiles[q] = 0;
		}
		for (int8_t q = 0; q < ((fieldsize + 1) * fieldsize / 2) / 8 + 1; q++) {
			left[q] = 255;
		}

		empty = size * size;
		sum = 0;
		for (int8_t ys = 1; ys <= size; ys++) {
			for (int8_t xs = 1; xs <= ys; xs++) {
				sum += xs * ys;
			}
		}
	}
	void operator=(Field other) {
		maximal = other.maximal;
		minimal = other.minimal;

		for (int8_t q = 0; q < 3 * fieldsize * fieldsize / 8 + 1; q++) {
			tiles[q] = other.tiles[q];
		}
		for (int8_t q = 0; q < ((fieldsize + 1) * fieldsize / 2) / 8 + 1; q++) {
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
		Pickpart(source, out, best, xs, ys);
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



int main() {
	Field<FieldSize> field = Pick<FieldSize>();

	cout << field.str() << endl;

	_getch();
	return 0;
}