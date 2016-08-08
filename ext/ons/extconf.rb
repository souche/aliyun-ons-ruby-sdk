# rubocop:disable Style/GlobalVars

# use mkmf-rice instead mkmf
require 'mkmf-rice'

# aliyun mq sdk dir
ALIYUN_MQ_CPP_SDK_DIR = File.expand_path('../aliyun-mq-cpp-sdk', __FILE__)

# header dirs to search
HEADER_DIRS = [
  '/opt/local/include',           # search /opt/local for macports
  '/usr/local/include',           # search /usr/local for people that installed from source
  RbConfig::CONFIG['includedir'], # check the ruby install locations
  File.join(ALIYUN_MQ_CPP_SDK_DIR, 'include'),
  '/usr/include',                 # finally fall back to /usr
].freeze

# library dirs to search
LIB_DIRS = [
  '/opt/local/lib',               # search /opt/local for macports
  '/usr/local/lib',               # search /usr/local for people that installed from source
  RbConfig::CONFIG['libdir'],     # check the ruby install locations
  File.join(ALIYUN_MQ_CPP_SDK_DIR, 'lib'),
  '/usr/lib',                     # finally fall back to /usr
].freeze

# configure cxxflags
$warnflags.gsub!(/-Wdeclaration-after-statement/, '')   # this flag is valid for C/ObjC but not for C++
$warnflags.gsub!(/-Wimplicit-function-declaration/, '') # this flag is valid for C/ObjC but not for C++
$warnflags << ' -Wno-ignored-qualifiers'

# configure various "with" options
dir_config('boost', HEADER_DIRS, LIB_DIRS)
dir_config('onsclient4cpp', HEADER_DIRS, LIB_DIRS)

# configure link libraries
abort 'libboost_chrono is missing, please install libboost_chrono' unless have_library('boost_chrono')
abort 'libboost_system is missing, please install libboost_system' unless have_library('boost_system')
abort 'libboost_thread is missing, please install libboost_thread' unless have_library('boost_thread')
abort unless have_library('onsclient4cpp')
abort unless have_func('rb_thread_call_without_gvl', 'ruby/thread.h')

# create Makefile
create_makefile('ons/ons')
