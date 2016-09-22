package = "torch-cjson"
version = "scm-1"
source = {
   url = "git://github.com/iamalbert/torch-cjson"
}
description = {
   summary = "load large JSON into LuaJIT",
   detailed = [[
   ]],
   homepage = "https://github.com/iamalbert/torch-cjson",
   license = "MIT"
}
dependencies = {
   "torch >= 7.0"
}
build = {
   type = "make",
   build_variables = {
      CXXFLAGS = "-std=c++14 -Wall -pedantic -O2 -I$(LUA_INCDIR)",
      LIBFLAG = "$(LIBFLAG)",
      LUA = "$(LUA)",
      LUA_BINDIR = "$(LUA_BINDIR)",
      LUA_INCDIR = "$(LUA_INCDIR)",
      LUA_LIBDIR = "$(LUA_LIBDIR)"
   },
   install_variables = {
      BINDIR = "$(BINDIR)",
      CONFDIR = "$(CONFDIR)",
      LIBDIR = "$(LIBDIR)",
      LUADIR = "$(LUADIR)",
      PREFIX = "$(PREFIX)"
   }
}
