#ifndef VIENNAUTILS_MEMORY_CAPTURE_HPP
#define VIENNAUTILS_MEMORY_CAPTURE_HPP


#include <cstdlib>
#include <malloc.h>
#include <stdint.h>

void* operator new (std::size_t size) throw (std::bad_alloc)
{ return malloc(size); }

void* operator new[] (std::size_t size) throw (std::bad_alloc)
{ return malloc(size); }

void operator delete( void *ptr ) throw()
{ free(ptr); }

void operator delete[]( void *ptr ) throw()
{ free(ptr); }


namespace viennautils
{

  class memory_capture
  {
  public:

    memory_capture( bool enable_ = true ) { if (enable_) enable(); }
    ~memory_capture() { if (enabled) set_default_hooks(); }

    void enable()
    {
//       __malloc_initialize_hook = my_init_hook;
      save_hooks();
      set_my_hooks();
      enabled = true;
    }

    int64_t allocated_memory() const { return total_allocated_bytes(); }

  private:

    bool enabled;


    typedef void * (*MallocHookFunctionType)(size_t, const void *);
    typedef void * (*ReallocHookFunctionType)(void *, size_t, const void *);
    typedef void * (*MemalignHookFunctionType)(size_t, size_t, const void *);
    typedef void (*FreeHookFunctionType)(void*, const void *);

    static MallocHookFunctionType & old_malloc_hook()
    {
      static MallocHookFunctionType hook;
      return hook;
    }

    static ReallocHookFunctionType & old_realloc_hook()
    {
      static ReallocHookFunctionType hook;
      return hook;
    }

    static MemalignHookFunctionType & old_memalign_hook()
    {
      static MemalignHookFunctionType hook;
      return hook;
    }

    static FreeHookFunctionType & old_free_hook()
    {
      static FreeHookFunctionType hook;
      return hook;
    }


//     static void * (*old_malloc_hook) (size_t, const void *);
//     static void * (*old_realloc_hook) (void *, size_t, const void *);
//     static void * (*old_memalign_hook) (size_t, size_t, const void *);
//     static void (*old_free_hook) (void*, const void *);

    static void save_hooks()
    {
      old_malloc_hook() = __malloc_hook;
      old_realloc_hook() = __realloc_hook;
      old_memalign_hook() = __memalign_hook;
      old_free_hook() = __free_hook;
    }

    static void set_my_hooks()
    {
      __malloc_hook = my_malloc_hook;
      __realloc_hook = my_realloc_hook;
      __memalign_hook = my_memalign_hook;
      __free_hook = my_free_hook;
    }

    static void set_default_hooks()
    {
      __malloc_hook = old_malloc_hook();
      __realloc_hook = old_realloc_hook();
      __memalign_hook = old_memalign_hook();
      __free_hook = old_free_hook();
    }



    static void
    my_init_hook (void)
    {
      save_hooks();
      set_my_hooks();
    }

    static void *
    my_malloc_hook (size_t size, const void *caller)
    {
      void *result;
      /* Restore all old hooks */
      set_default_hooks();
      /* Call recursively */
      result = malloc (size);

      /* Save underlying hooks */
      save_hooks();

      {
        allocated_memories()[result] = size;
        total_allocated_bytes() += size;
      }

      /* Restore our own hooks */
      set_my_hooks();
      return result;
    }

    static void *my_realloc_hook (void * ptr, size_t size, const void *)
    {
      void *result;
      /* Restore all old hooks */
      set_default_hooks();
      /* Call recursively */
      result = realloc(ptr, size);

      /* Save underlying hooks */
      save_hooks();

      {
        std::map<void*,size_t>::iterator sit = allocated_memories().find(ptr);
        if (sit != allocated_memories().end())
        {
          total_allocated_bytes() += (size - sit->second);
          sit->second = size;
        }
        else
        {
          allocated_memories()[result] = size;
          total_allocated_bytes() += size;
        }
      }

      /* Restore our own hooks */
      set_my_hooks();
      return result;
    }


    static void * my_memalign_hook (size_t alignment, size_t size, const void *)
    {
      void *result;
      /* Restore all old hooks */
      set_default_hooks();
      /* Call recursively */
      result = memalign (alignment, size);

      /* Save underlying hooks */
      save_hooks();

      {
        allocated_memories()[result] = size;
        total_allocated_bytes() += size;
      }

      /* Restore our own hooks */
      set_my_hooks();
      return result;
    }



    static void
    my_free_hook (void *ptr, const void *caller)
    {
      if (!ptr)
        return;

      /* Restore all old hooks */
      set_default_hooks();
      /* Call recursively */
      free (ptr);

      /* Save underlying hooks */
      save_hooks();

      {
        std::map<void*,size_t>::iterator sit = allocated_memories().find(ptr);
        if (sit != allocated_memories().end())
        {
          total_allocated_bytes() -= sit->second;
          allocated_memories().erase(sit);
        }
//         else
//           std::cout << "Freeing a not allocated memory block, this should NOT happen" << std::endl;
      }

      /* Restore our own hooks */
      set_my_hooks();
    }


    static int64_t & total_allocated_bytes()
    {
      static int64_t memory = 0;
      return memory;
    }

    static std::map<void*, size_t> & allocated_memories()
    {
      static std::map<void*, size_t> memory_map;
      return memory_map;
    }

  };


}






#endif
