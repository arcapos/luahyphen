/*
 * Copyright (c) 2016 - 2018 Micro Systems Marc Balmer, CH-5073 Gipf-Oberfrick
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Micro Systems Marc Balmer nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* Hyphenation for Lua */

#include <hyphen.h>
#include <lua.h>
#include <lauxlib.h>
#include <stdlib.h>


#include "luahyphen.h"

static int
hyphen_load(lua_State *L)
{
	HyphenDict **dict;

	dict = lua_newuserdata(L, sizeof(HyphenDict *));
	*dict = hnj_hyphen_load(luaL_checkstring(L, 1));
	if (*dict)
		luaL_setmetatable(L, DICT_METATABLE);
	else
		lua_pushnil(L);
	return 1;
}

#if defined(HAVE_LOAD_FILE)
static int
hyphen_load_file(lua_State *L)
{
	HyphenDict **dict;
	luaL_Stream *stream;

	stream = luaL_checkudata(L, 1, LUA_FILEHANDLE);
	luaL_argcheck(L, stream->f != NULL, 1, "invalid file handle");
	dict = lua_newuserdata(L, sizeof(HyphenDict *));

	*dict = hnj_hyphen_load_file(stream->f);
	if (*dict)
		luaL_setmetatable(L, DICT_METATABLE);
	else
		lua_pushnil(L);
	return 1;
}
#endif

static int
dict_hyphenate(lua_State *L)
{
	HyphenDict **dict;
	const char *word;
	char *hyphens;
	char *hyphenated_word;
	size_t word_size;
	char **rep;
	int *pos, *cut;

	dict = luaL_checkudata(L, 1, DICT_METATABLE);
	word = luaL_checklstring(L, 2, &word_size);
	rep = NULL;
	pos = NULL;
	cut = NULL;

	hyphenated_word = malloc(2 * word_size);
	if (hyphenated_word == NULL)
		return luaL_error(L, "out of memory");

	hyphens = malloc(word_size + 5);
	if (hyphens == NULL)
		return luaL_error(L, "out of memory");
	hnj_hyphen_hyphenate2(*dict, word, word_size, hyphens, hyphenated_word,
	    &rep, &pos, &cut);
	lua_pushstring(L, hyphenated_word);
	free(hyphens);
	free(hyphenated_word);
	return 1;
}

static int
dict_free(lua_State *L)
{
	HyphenDict **dict;

	dict = luaL_checkudata(L, 1, DICT_METATABLE);
	if (*dict != NULL) {
		hnj_hyphen_free(*dict);
		*dict = NULL;
	}
	return 0;
}

int
luaopen_hyphen(lua_State *L)
{
	struct luaL_Reg luahyphen[] = {
		{ "load",	hyphen_load },
#if defined(HAVE_LOAD_FILE)
		{ "load_file",	hyphen_load_file },
#endif
		{ "hyphenate",	dict_hyphenate },
		{ NULL, NULL }
	};
	struct luaL_Reg dict_methods[] = {
		{ "hyphenate",	dict_hyphenate },
		{ "free",	dict_free },
		{ "__gc",	dict_free },
		{ NULL, NULL }
	};

	/* The DICT metatable */
	if (luaL_newmetatable(L, DICT_METATABLE)) {
		luaL_setfuncs(L, dict_methods, 0);
		lua_pushliteral(L, "__index");
		lua_pushvalue(L, -2);
		lua_settable(L, -3);
		lua_pushliteral(L, "__metatable");
		lua_pushliteral(L, "must not access this metatable");
		lua_settable(L, -3);
	}
	lua_pop(L, 1);
	luaL_newlib(L, luahyphen);
	lua_pushliteral(L, "_COPYRIGHT");
	lua_pushliteral(L, "Copyright (C) 2016 - 2018 micro systems "
	    "marc balmer");
	lua_settable(L, -3);
	lua_pushliteral(L, "_DESCRIPTION");
	lua_pushliteral(L, "Hyphenation for Lua");
	lua_settable(L, -3);
	lua_pushliteral(L, "_VERSION");
	lua_pushliteral(L, "hyphenation 1.2.0");
	lua_settable(L, -3);
	return 1;
}
