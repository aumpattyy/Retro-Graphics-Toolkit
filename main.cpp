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
	Copyright Sega16 (or whatever you wish to call me) (2012-2015)
*/
#include <unistd.h>
#include <libgen.h>
#include "class_global.h"
#include "gui.h"
#include "callbacktilemaps.h"
#include "runlua.h"
const char*rtVersionStr="Retro Graphics Toolkit v0.8 RC2";
editor*window=nullptr;

//From lua.c
/*
** Create the 'arg' table, which stores all arguments from the
** command line ('argv'). It should be aligned so that, at index 0,
** it has 'argv[script]', which is the script name. The arguments
** to the script (everything after 'script') go to positive indices;
** other arguments (before the script name) go to negative indices.
** If there is no script name, assume interpreter's name as base.
*/
static void createargtable (lua_State *L, char **argv, int argc, int script) {
	int i, narg;
	if (script == argc) script = 0;  /* no script name? */
	narg = argc - (script + 1);  /* number of positive indices */
	lua_createtable(L, narg, script + 1);
	for (i = 0; i < argc; i++) {
		lua_pushstring(L, argv[i]);
		lua_rawseti(L, -2, i - script);
	}
	lua_setglobal(L, "arg");
}
#ifdef _WIN32
#define _chdir chdir
#endif
int main(int argc, char **argv){
#ifdef _WIN32
	char olddirname[MAX_PATH];
	getcwd(olddirname,sizeof(olddirname));
#else
	const char*olddirname=get_current_dir_name();
#endif
	char*tmp=strdup(argv[0]);
	chdir(dirname(tmp));
	free((void*)tmp);
	int headless=0,useExampleFolder=0;
	if(argc>=3){
		if(!strcmp(argv[1],"--headless"))
			headless=1;
		if(!strcmp(argv[1],"--headless-examples")){
			headless=1;
			useExampleFolder=1;
		}
	}
	Fl::visual(FL_DOUBLE|FL_INDEX);
	fl_register_images();
#if !defined(WIN32) && !defined(__APPLE__)
	Fl_File_Icon::load_system_icons();
#endif
	if(headless){
		// Run Lua script.
		lua_State*Lconf=createLuaState();
		runLua(Lconf,"configHeadless.lua");
		lua_State*Lheadless=createLuaState();
		// Create arg table just like Lua standalone
		std::string pathbld="./headlessExamples/";
		char*examplePath;
		if(useExampleFolder){
			pathbld+=argv[2];
			examplePath=realpath(pathbld.c_str(),nullptr);
			if(!examplePath){
				fl_alert("realpath failure");
				return 1;
			}
			argv[2]=examplePath;
		}
		createargtable(Lheadless,argv,argc,2);
		chdir(olddirname);
#ifndef _WIN32
		free((void*)olddirname);
#endif
		if(useExampleFolder){
			runLua(Lheadless,examplePath);
			free(examplePath);
		}else
			runLua(Lheadless,argv[2]);
		return 0;
	}else{
		window = new editor(800,600,rtVersionStr);
		printf("Welcome to Retro graphics Toolkit\nWritten by Sega16/Nintendo8\nBuilt on %s %s\n",__DATE__,__TIME__);
		window->resizable(window);
		updateTileSelectAmt();
		updatePlaneTilemapMenu(curProjectID,window->planeSelect);
		//// For a nicer looking browser under Linux, call Fl_File_Icon::load_system_icons();
		//// (If you do this, you'll need to link with fltk_images)
		//// NOTE: If you do not load the system icons, the file chooser will still work, but
		////       no icons will be shown. However, this means you do not need to link in the
		////       fltk_images library, potentially reducing the size of your executable.
		//// Loading the system icons is not required by the OSX or Windows native file choosers.
		window->show(argc,argv);
		return Fl::run();
	}
}
