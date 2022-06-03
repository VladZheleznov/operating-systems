#ifndef Vector_data_h
#define Vector_data_h
struct Vector_data
{
    double* array_;
    size_t size_, k_;
    double sum_, accuracy_;

    Vector_data(double* array, size_t size, double accuracy)
    {
        this->array_ = array;
        this->size_ = size;
        this->sum_ = 0;
        this->accuracy_ = accuracy;
    }

    ~Vector_data()
    {
        delete[] array_;
    }
};
#endif