#ifndef POCKET_H
#define POCKET_H
struct Pocket
{
	double N_;
	size_t size_;
	double* array_;

	Pocket(size_t size, double* array, double N) : 
		size_(size), array_(array), N_(N) {}
};
#endif