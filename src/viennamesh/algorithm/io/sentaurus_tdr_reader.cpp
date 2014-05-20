#ifdef VIENNAMESH_WITH_TDR

#include "viennamesh/algorithm/io/sentaurus_tdr_reader.hpp"

namespace viennamesh
{
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

    char * buf = new char[a.getDataType().getSize()+1];
    a.read(a.getDataType(),buf);
    buf[a.getDataType().getSize()]='\0';

    string result(buf);
    delete[] buf;

    return result;
  }

}

#endif
