#ifndef VIENNAUTILS_PROGRESS_TRACKER_HPP
#define VIENNAUTILS_PROGRESS_TRACKER_HPP

namespace viennautils
{

  template<typename TriggerT>
  class progress_tracker
  {
  public:

    progress_tracker(double max_, int trigger_count, TriggerT const & trigger_) : trigger(trigger_), max(max_),  next_trigger_step(max/trigger_count), count(0), next_trigger(next_trigger_step) {}

    void increase(int to_increase = 1)
    {
      count += to_increase;
      if (count > next_trigger)
      {
        trigger( status() );
        next_trigger += next_trigger_step;
      }
    }

    double status() const
    {
      return count/max;
    }

  private:
    TriggerT trigger;

    double max;
    double next_trigger_step;

    double count;
    double next_trigger;
  };

}

#endif
