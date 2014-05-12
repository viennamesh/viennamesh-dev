#ifdef VIENNAMESH_WITH_TDR

#include "viennamesh/algorithm/io/sentaurus_tdr_reader.hpp"

namespace viennamesh
{
  tree_t tree;
  hid_t parent=-1;


  int globalattribute=0;
  std::map<string,attributeinfo_c> globalattributes;
  std::set<string> removeattribute;
  std::set<string> removesegment;
  double cutsegmentbelow=-1000000;
  double cutsegmentabove=-1000000;
  string addsegment;
  string addmaterial;

  void iterate_attrs(H5Object &element,int depth,string type)
  {
    const hid_t n=element.getNumAttrs();
    for (hid_t i=0; i<n; i++)
    {
      Attribute attr=element.openAttribute(i);
      string name=attr.getName();
      printf("% *s%s : A\n",depth*4,"",name.c_str());
    }
  }

  void iterate_elements(CommonFG &element,int depth,string type)
  {
    const hsize_t n=element.getNumObjs();
    for (hsize_t i=0; i<n; i++)
    {
      string name=element.getObjnameByIdx(i);
      H5G_stat_t statbuf;
      element.getObjinfo (name, statbuf);
      if (statbuf.type==H5G_GROUP)
      {
        Group g=element.openGroup(name);
        if (g.getNumObjs())
        {
          printf("% *s%s : %s%s\n",depth*4,"",name.c_str(),type.c_str(),"G");
          printf("% *s%ld\n",depth*4,"",g.getNumAttrs());
          iterate_attrs(g,depth);
        }
        iterate_elements(g,depth+1,type+"G");
        continue;
      }
      if (statbuf.type==H5G_DATASET)
      {
        printf("% *s%s : %s%s\n",depth*4,"",name.c_str(),type.c_str(),"D");
        DataSet d=element.openDataSet(name);
        printf("% *s%ld\n",depth*4,"",d.getNumAttrs());
        iterate_attrs(d,depth);
        //iterate_elements(d,depth+1,type+"D");
        continue;
      }
      if (statbuf.type==H5G_TYPE)
      {
        printf("% *s%s : %s%s\n",depth*4,"",name.c_str(),type.c_str(),"T");
        printf("TYPE, THROW\n");
        throw;
        continue;
      }
      printf("% *s%s : %s\n",depth*4,"",name.c_str(),"?");
      printf("UNKNOWN, THROW\n");
      throw;
    }
  }

  void check_class(H5T_class_t t)
  {
    switch (t)
    {

      case H5T_NO_CLASS     :std::cerr << "H5T_NO_CLASS\n"; break;
      case H5T_INTEGER     :std::cerr << "H5T_INTEGER\n"; break;
      case H5T_FLOAT     :std::cerr << " H5T_FLOAT \n"; break;
      case H5T_TIME      :std::cerr << "H5T_TIME \n"; break;
      case H5T_STRING       :std::cerr << "H5T_STRING  \n"; break;
      case H5T_BITFIELD     :std::cerr << "H5T_BITFIELD\n"; break;
      case H5T_OPAQUE      :std::cerr << "H5T_OPAQUE \n"; break;
      case H5T_COMPOUND     :std::cerr << "H5T_COMPOUND\n"; break;
      case H5T_REFERENCE     :std::cerr << "H5T_REFERENCE\n"; break;
      case H5T_ENUM         :std::cerr << "H5T_ENUM    \n"; break;
      case H5T_VLEN        :std::cerr << "H5T_VLEN   \n"; break;
      case H5T_ARRAY      :std::cerr << "H5T_ARRAY \n"; break;
      case H5T_NCLASSES     :std::cerr << "H5T_NCLASSES\n"; break;
      default :std::cerr << "UNKNOWN\n"; break;
    }
  }

  int read_int(const H5Object &g, const string name)
  {
    int i;
    Attribute a=g.openAttribute(name);
    if (a.getTypeClass()!=H5T_INTEGER)
      mythrow("Wrong class in atrribute");
    a.read(a.getDataType(),&i);
    return i;
  }
  double read_double(const H5Object &g, const string name)
  {
    double i;
    Attribute a=g.openAttribute(name);
    //if (a.getTypeClass()!=H5T_DOUBLE)
    if (a.getTypeClass()!=H5T_FLOAT)
      mythrow("Wrong class " << typeid(a.getTypeClass()).name() << " in atrribute");
    a.read(a.getDataType(),&i);
    return i;
  }
  //string read_string(const Group &g, const string name)
  string read_string(const H5Object &g, const string name)
  {
    Attribute a=g.openAttribute(name);
    if (a.getTypeClass()!=H5T_STRING)
      mythrow("Wrong class in atrribute");

    char buf[a.getDataType().getSize()+1];
    a.read(a.getDataType(),buf);
    buf[a.getDataType().getSize()]='\0';

    return string(buf);
  }

}

#endif
