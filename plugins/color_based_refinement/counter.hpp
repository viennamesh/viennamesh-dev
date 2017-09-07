#ifndef COUNTER_HPP
#define COUNTER_HPP

template<typename T>
class Counter
{
    public:
        Counter() {++count;}
        Counter(const Counter&) {++count;}
        ~Counter() {--count;}
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