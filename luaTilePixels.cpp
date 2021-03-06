/*
	This file is part of Retro Graphics Toolkit

	Retro Graphics Toolkit is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or any later version.

	Retro Graphics Toolkit is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Retro Graphics Toolkit. If not, see <http://www.gnu.org/licenses/>.
	Copyright Sega16 (or whatever you wish to call me) (2012-2019)
*/
#include "luaTilePixels.hpp"
#include "luaTilePixelsRow.hpp"
#include "luaTilemap.hpp"
#include "luaHelpers.hpp"
#include "project.h"
#include "dub/dub.h"
#include "gui.h"
static int tilePixels__get_(lua_State *L) {
	int type = lua_type(L, 2);

	if (type == LUA_TNUMBER) {
		int k = luaL_checkinteger(L, 2) - 1;

		const size_t *idxPtr = (const size_t*)lua_touserdata(L, 1);
		size_t idx = *idxPtr;
		size_t tileIDX = idxPtr[1];

		if (k >= 0 && k < projects[idx].tileC->height()) {
			luaopen_TilePixelsRow(L, idx, tileIDX, k);
			return 1;
		}
	}

	return 0;
}

static int tilePixels__len_(lua_State *L) {
	getProjectIDX
	lua_pushinteger(L, projects[projectIDX].tileC->height());
	return 1;
}

static int tilePixels___tostring(lua_State *L) {
	lua_pushstring(L, "tilePixels table");
	return 1;
}

static const struct luaL_Reg tilePixels_member_methods[] = {
	{ "__index", tilePixels__get_       },
	{ "__len", tilePixels__len_       },
	{ "__tostring", tilePixels___tostring  },
	{ "deleted", dub::isDeleted    },
	{ NULL, NULL},
};

int luaopen_TilePixels(lua_State *L, size_t projectIDX, size_t tileIDX) {
	// Create the metatable which will contain all the member methods
	luaL_newmetatable(L, "tilePixels");
	// <mt>

	// register member methods
	dub::fregister(L, tilePixels_member_methods);
	dub::setup(L, "tilePixels");
	// setup meta-table
	size_t* idxUserData = (size_t*)lua_newuserdata(L, sizeof(size_t) * 2);
	luaL_getmetatable(L, "tilePixels");
	idxUserData[0] = projectIDX;
	idxUserData[1] = tileIDX;

	lua_setmetatable(L, -2);
	return 1;
}

