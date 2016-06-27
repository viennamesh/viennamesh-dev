#include "algorithm.hpp"
#include "context.hpp"

void input_parameter::unset()
{
  if (input)
    input->release();

  if (source_algorithm)
  {
    source_algorithm->release();
    source_name.clear();
  }
}

void input_parameter::set(viennamesh_data_wrapper input_)
{
  unset();
  input = input_;
  input->retain();
}

void input_parameter::link(viennamesh_algorithm_wrapper source_algorithm_, std::string source_name_)
{
  unset();
  source_algorithm = source_algorithm_;
  source_algorithm->retain();
  source_name = source_name_;
}

viennamesh_data_wrapper input_parameter::unpack() const
{
  if (input)
    return input;

  if (source_algorithm)
    return source_algorithm->get_output(source_name);

  return 0;
}






void viennamesh_algorithm_wrapper_t::init()
{
  algorithm_template()->init(this);
}


void viennamesh_algorithm_wrapper_t::run()
{
  algorithm_template()->run(this);
}

viennamesh_context viennamesh_algorithm_wrapper_t::context()
{
  return algorithm_template_->context();
}

std::string const & viennamesh_algorithm_wrapper_t::type()
{
  return algorithm_template()->type();
}

void viennamesh_algorithm_wrapper_t::delete_this()
{
#ifdef VIENNAMESH_BACKEND_RETAIN_RELEASE_LOGGING
  std::cout << "Delete algorithm at " << this << std::endl;
#endif
  algorithm_template()->delete_algorithm( internal_algorithm() );
  delete this;
}



void viennamesh_algorithm_wrapper_t::clear_inputs()
{
  inputs.clear();
}

void viennamesh_algorithm_wrapper_t::unset_input(std::string const & name)
{
  InputMapType::iterator it = inputs.find(name);
  if (it != inputs.end())
    inputs.erase(it);
}

void viennamesh_algorithm_wrapper_t::set_input(std::string const & name, viennamesh_data_wrapper input)
{
  if (input->context() != context())
    VIENNAMESH_ERROR(VIENNAMESH_ERROR_DIFFERENT_CONTEXT, "");

  inputs[name].set(input);
}

void viennamesh_algorithm_wrapper_t::link_input(std::string const & name, viennamesh_algorithm_wrapper source_algorithm, std::string const & source_name)
{
  if (source_algorithm->context() != context())
    VIENNAMESH_ERROR(VIENNAMESH_ERROR_DIFFERENT_CONTEXT, "");

  inputs[name].link(source_algorithm, source_name);
}

viennamesh_data_wrapper viennamesh_algorithm_wrapper_t::get_input(std::string const & name)
{
  InputMapType::const_iterator it = inputs.find(name);

  viennamesh_data_wrapper result = 0;

  if (it == inputs.end())
  {
    if (default_source)
      result = default_source->get_output(name);
  }
  else
    result = it->second.unpack();

  if (result)
  {
    result->retain();
    return result;
  }

  return 0;
}

viennamesh_data_wrapper viennamesh_algorithm_wrapper_t::get_input(std::string const & name,
                                            std::string const & type_name)
{
  viennamesh_data_wrapper input = get_input(name);

  if (!input)
    return 0;

  if (input->type_name() == type_name)
    return input;


  input->release();

  viennamesh::backend::info(1) << "Requested input \"" << name << "\" of type \"" << type_name << "\" but input is of type \"" << input->type_name() << "\"";

  viennamesh_data_wrapper result = context()->convert_to(input, type_name);

  viennamesh::backend::info(1) << "; conversion: " << ((result)?"success":"failed") << std::endl;

  return result;
}




void viennamesh_algorithm_wrapper_t::clear_outputs()
{
  for (OutputMapType::iterator it = outputs.begin(); it != outputs.end(); ++it)
    (*it).second->release();

  outputs.clear();
}

void viennamesh_algorithm_wrapper_t::set_output(std::string const & name, viennamesh_data_wrapper output)
{
  if (output->context() != context())
    VIENNAMESH_ERROR(VIENNAMESH_ERROR_DIFFERENT_CONTEXT, "");

  OutputMapType::iterator it = outputs.find(name);
  if (it != outputs.end())
    it->second->release();

  outputs[name] = output;
  output->retain();
}


viennamesh_data_wrapper viennamesh_algorithm_wrapper_t::get_output(std::string const & name)
{
  OutputMapType::iterator it = outputs.find(name);
  if (it == outputs.end())
    return 0;

  return it->second;
}

viennamesh_data_wrapper viennamesh_algorithm_wrapper_t::get_output(std::string const & name,
                                        std::string const & type_name)
{
  OutputMapType::iterator it = outputs.find(name);
  if (it == outputs.end())
    return 0;

  if (it->second->type_name() == type_name)
    return it->second;

  return context()->convert_to(it->second, type_name);
}
