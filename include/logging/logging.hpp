/**
 * @date 19 déc. 2009
 * @todo comment
 */

#ifndef LOGGER_HXX_
#define LOGGER_HXX_

#define BOOST_FILESYSTEM_VERSION 2

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/filters.hpp>

namespace logging {

  void init();

} // namespace logging

#define __fatalM(module_m)  __logML(module_m,fatal)
#define __fatal             __logML(main,fatal)
#define __errorM(module_m)  __logML(module_m,error)
#define __error             __logML(main,error)
#define __warnM(module_m)   __logML(module_m,warning)
#define __warn              __logML(main,warning)
#define __infoM(module_m)   __logML(module_m,info)
#define __info              __logML(main,info)
#define __debugM(module_m)  __logML(module_m,debug)
#define __debug             __logML(main,debug)

#define __logL(level)       __logML(main,level)

#define __logML(module_m, level_m) BOOST_LOG_TRIVIAL(level_m) << #module_m \

#endif /* LOGGER_HXX_ */
