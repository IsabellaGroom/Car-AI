#include "t_Array.h"

template <class T>
t_Array<T>::t_Array(int size)
{
	my_array = new T[size];
}
