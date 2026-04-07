#include "lua.hpp"
#include "Astart.h"
#include <string.h>

static int astar_CheckPoint(lua_State* L)
{
    Map* map = *(Map**)luaL_checkudata(L, 1, "gge_astar");
    if (!map) return 0;
    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);

    if (map->data && x >= 0 && x < map->width && y >= 0 && y < map->height)
    {
        int n = y * map->width + x;
        lua_pushboolean(L, map->data[n] == 0);
    }
    else
        lua_pushboolean(L, 0);

    return 1;
}

static int astar_GetPoint(lua_State* L)
{
    Map* map = *(Map**)luaL_checkudata(L, 1, "gge_astar");
    if (!map) return 0;
    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);

    if (map->data && x >= 0 && x < map->width && y >= 0 && y < map->height)
    {
        int n = y * map->width + x;
        lua_pushinteger(L, map->data[n]);
    }
    else
        lua_pushinteger(L, -1);

    return 1;
}

static int astar_SetPoint(lua_State* L)
{
    Map* map = *(Map**)luaL_checkudata(L, 1, "gge_astar");
    if (!map) return 0;
    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);
    int v = luaL_checkinteger(L, 4);

    if (map->data && x >= 0 && x < map->width && y >= 0 && y < map->height)
    {
        int n = y * map->width + x;
        map->data[n] = v;
    }
    return 0;
}

static int astar_GetPath(lua_State* L)
{
    Map* map = *(Map**)luaL_checkudata(L, 1, "gge_astar");
    if (!map || !map->data) { lua_newtable(L); return 1; }
    POINT nodeStart, nodeEnd, nodeCur;
    nodeStart.x = luaL_checkinteger(L, 2);
    nodeStart.y = luaL_checkinteger(L, 3);
    nodeEnd.x = luaL_checkinteger(L, 4);
    nodeEnd.y = luaL_checkinteger(L, 5);
    bool mode = luaL_optinteger(L, 6, 0);
    nodeCur = nodeStart;

    lua_newtable(L);
    if (nodeEnd.x < 0 || nodeEnd.x >= map->width || nodeEnd.y < 0 || nodeEnd.y >= map->height)
        return 1;
    if (FindPath(map, &nodeStart, &nodeEnd, mode))
    {
        int i = 1;
        for (;;)
        {
            if (NextPath(map, &nodeCur))
            {
                lua_newtable(L);
                lua_pushinteger(L, nodeCur.x);
                lua_setfield(L, -2, "x");
                lua_pushinteger(L, nodeCur.y);
                lua_setfield(L, -2, "y");
                lua_rawseti(L, -2, i++);
                if (nodeCur.x == nodeEnd.x && nodeCur.y == nodeEnd.y)
                    break;
            }
            else
                break;
        };
    }
    return 1;
}

static int astar_new(lua_State* L)
{
    int w = luaL_checkinteger(L, 1);
    int h = luaL_checkinteger(L, 2);

    if (w <= 0 || h <= 0)
        return luaL_error(L, "invalid map size: %dx%d", w, h);

    Map* map = MapCreate(w, h, NULL);
    if (!map)
        return luaL_error(L, "failed to create map");

    /* 创建 userdata 并 **立即** 设置 metatable，
       确保后续任何 luaL_error/longjmp 都能触发 __gc 清理 */
    Map** ud = (Map**)lua_newuserdata(L, sizeof(Map*));
    *ud = map;
    luaL_getmetatable(L, "gge_astar");
    lua_setmetatable(L, -2);

    map->data = (unsigned char*)malloc(map->size);
    if (!map->data)
        return luaL_error(L, "failed to allocate map data (%u bytes)", map->size);

    if (lua_isuserdata(L, 3) && lua_isnumber(L, 4))
    {
        const void* data = (char*)lua_topointer(L, 3);
        size_t len = luaL_checkinteger(L, 4);
        if (len == map->size)
            memcpy(map->data, data, len);
        else
            luaL_error(L, "data error");
    }
    else if (lua_isstring(L, 3))
    {
        size_t len;
        const void* data = (const void*)luaL_checklstring(L, 3, &len);
        if (len == map->size)
            memcpy(map->data, data, len);
    }
    return 1;
}

static int astar_gc(lua_State* L)
{
    Map** ud = (Map**)luaL_checkudata(L, 1, "gge_astar");
    Map* map = *ud;

    /* 空指针守卫：防止双重释放 */
    if (!map)
        return 0;

    /* 立即清除 userdata 中的指针，阻断重入 */
    *ud = NULL;

    /* 先释放 data 缓冲区 */
    if (map->data)
    {
        free(map->data);
        map->data = NULL;
    }

    /* 无论 data 是否为 NULL，都必须销毁 Map 自身（list + node + map） */
    MapDestroy(map);

    return 0;
}

extern "C"
LUALIB_API int luaopen_gastar(lua_State * L)
{
    luaL_Reg methods[] = {
        {"__gc", astar_gc},
        {"GetPath", astar_GetPath},
        {"CheckPoint", astar_CheckPoint},
        {"GetPoint", astar_GetPoint},
        {"SetPoint", astar_SetPoint},
        {NULL, NULL},
    };
    luaL_newmetatable(L, "gge_astar");
    luaL_setfuncs(L, methods, 0);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    lua_pop(L, 1);

    lua_pushcfunction(L, astar_new);
    return 1;
}
