#include "viennamesh/utils/logger.hpp"

using namespace viennamesh;

int main()
{

  Logger log;

  log.register_callback( new OStreamCallback<CoutColorFormater>(std::cout) );


  log.log<info_tag>(5, "bla", "blub\n");

  {
    LoggingStack ls(log);
    log.info(5, "bla") << "Haallo!!" << " " << 5 << " das ist ein Test" << std::endl;


    log.warning(5, "matrix_assebmly") << "Ich bin eine Warning" << std::endl;
    log.warning(5) << "Ich bin eine Warning" << std::endl << "Noch ne Warnung" << std::endl;



    log.error(1, "bla") << "Ich bin ein Error" << std::endl;

    log.error(1, "bla") << "TOT" << std::endl;

    log.debug(1, "bla") << "TOT" << std::endl;
  }

  log.info(5, "bla") << "Und aus" << std::endl;

//   log.increase_indentation();

//   log.decrease_indentation();

  return 0;
}
