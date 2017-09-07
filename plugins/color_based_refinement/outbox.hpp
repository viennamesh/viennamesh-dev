#ifndef OUTBOX_HPP
#define OUTBOX_HPP

#include "counter.hpp"

class Outbox : private Counter<Outbox>
{
    public:
        //Constructor
        Outbox()
        {
            /*std::cout << std::endl << "========================================================================================" << std::endl;
            std::cout << "Outbox Constructor for partition " << no_outboxes << std::endl;
            ++no_outboxes;
            //std::cout << "  " << no_outboxes << std::endl;
            std::cout << "========================================================================================" << std::endl;*/

            //++no_outboxes;
        }

        Outbox(bool no_new_instance)
        {
            //std::cout << "Outbox Constructor for resizing" << std::endl;
            //std::cout << "  " << no_outboxes << std::endl;
        }

        Outbox & operator() (int target_part_id, int v1, int v2, int new_vert)
        {
            data.push_back(target_part_id);
            data.push_back(v1);
            data.push_back(v2);
            data.push_back(new_vert);
        }

        int & operator[] (int v)
        {
            return data[v];
        }

        int size()
        {
            return data.size();
        }

        int num_verts()
        {
            return data.size()/4;
        }

        int verts_in_part(int part_id)
        {
            int num_verts = 0;
            for (size_t i = 0; i < this->num_verts(); ++i)
            {
                if ( data[4*i] == part_id )
                {
                    ++num_verts;
                }
            }

            return num_verts;
        }

        using Counter<Outbox>::howMany;

    private:
        std::vector<int> data;
};

#endif //OUTBOX_HPP
