#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include <stdio.h>
#include <stdlib.h>
#include "ffmpeg.h"

static int out2jpeg( lua_State *L )
{
	size_t videoLength;
	size_t jpegLength;
	lua_Number len = luaL_checknumber(L, 1);
	const char *video = (const  char *)luaL_checklstring( L, 2, &videoLength );
	const char *jpeg = (const  char *)luaL_checklstring( L, 3, &jpegLength );

	if (videoLength <= 0 || jpegLength <= 0)
	{
		lua_pushinteger(L, -2);
	}
        int i = process(len, video, jpeg);
	lua_pushinteger(L, i);
        return 1;
}


static const struct luaL_reg cpt[] = {

        {"out2jpeg", out2jpeg},
        {NULL, NULL}

};

/* register library */
LUALIB_API int luaopen_video2mpeg( lua_State *L ){
        luaL_openlib( L, "video2mpeg", cpt, 0 );
        return 1;
}

