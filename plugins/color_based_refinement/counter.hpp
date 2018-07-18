#ifndef COUNTER_HPP
#define COUNTER_HPP

template<typename T>
class Counter
{
    public:
        Counter() {
        #pragma omp atomic
            ++count;
        }
        Counter(const Counter&) {
        #pragma omp atomic
            ++count;
        }
        ~Counter() {
        #pragma omp atomic
            --count;
        }
        static size_t howMany()
        {
            return count;
        }

    private:
        static size_t count;
};

template<typename T>
size_t Counter<T>::count = 0;

#endif