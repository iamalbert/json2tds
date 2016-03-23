package = "json2tds"
version = "1.0-0"
source = {
   url = "git://github.com/iamalbert/json2tds"
}
description = {
   summary = "load Json into Lua",
   detailed = [[
   ]],
   homepage = "https://github.com/iamalbert/json2tds",
   license = "MIT"
}
dependencies = {
   "torch >= 7.0"
}
build = {
   type = "make",
   build_variables = {
      CXXFLAGS = "-std=c++11 -Wall -pedantic -O2 -I$(LUA_INCDIR)",
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