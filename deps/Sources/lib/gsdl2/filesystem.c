#include "gge.h"
#include "SDL_filesystem.h"

static int LUA_GetBasePath(lua_State *L)
{
    char *str = SDL_GetBasePath();

    if (str)
    {
        lua_pushstring(L, str);
        SDL_free(str);
        return 1;
    }
    return 0;
}

static char* g_cached_pref_path = NULL;
static char g_cached_org[128] = {0};
static char g_cached_app[128] = {0};

static int LUA_GetPrefPath(lua_State *L)
{
    const char *organization = luaL_checkstring(L, 1);
    const char *application = luaL_optstring(L, 2, organization);

    if (g_cached_pref_path && 
        SDL_strncmp(g_cached_org, organization, sizeof(g_cached_org)) == 0 && 
        SDL_strncmp(g_cached_app, application, sizeof(g_cached_app)) == 0)
    {
        lua_pushstring(L, g_cached_pref_path);
        return 1;
    }

    char *str = SDL_GetPrefPath(organization, application);
    if (str)
    {
        if (g_cached_pref_path) {
            SDL_free(g_cached_pref_path);
        }
        g_cached_pref_path = SDL_strdup(str);
        SDL_strlcpy(g_cached_org, organization, sizeof(g_cached_org));
        SDL_strlcpy(g_cached_app, application, sizeof(g_cached_app));

        lua_pushstring(L, str);
        SDL_free(str);
        return 1;
    }

    return 0;
}

static const luaL_Reg sdl_funcs[] = {
    {"GetBasePath", LUA_GetBasePath},
    {"GetPrefPath", LUA_GetPrefPath},
    {NULL, NULL}};

int bind_filesystem(lua_State *L)
{
    luaL_setfuncs(L, sdl_funcs, 0);
    return 0;
}