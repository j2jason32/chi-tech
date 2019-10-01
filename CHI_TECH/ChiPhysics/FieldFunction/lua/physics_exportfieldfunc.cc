#include "ChiLua/chi_lua.h"

#include "ChiPhysics/chi_physics.h"
#include "ChiPhysics/FieldFunction/fieldfunction.h"

#include <chi_log.h>

extern ChiPhysics chi_physics_handler;
extern ChiLog     chi_log;


//#############################################################################
/** Exports a field function to VTK format.
 *
\param FFHandle int Global handle to the field function.
\param BaseName char Base name for the exported file.

\ingroup LuaFieldFunc
\author Jan*/
int chiExportFieldFunctionToVTK(lua_State *L)
{
  int num_args = lua_gettop(L);
  if ((num_args < 2) or (num_args > 3))
    LuaPostArgAmountError("chiExportFieldFunctionToVTK", 2, num_args);

  int ff_handle = lua_tonumber(L,1);
  const char* base_name = lua_tostring(L,2);
  const char* field_name = base_name;
  if (num_args == 3)
    field_name = lua_tostring(L,3);

  //======================================================= Getting solver
  chi_physics::FieldFunction* ff;
  try
  {
    ff = chi_physics_handler.fieldfunc_stack.at(ff_handle);
  }
  catch(const std::out_of_range& o)
  {
    chi_log.Log(LOG_ALLERROR)
      << "Invalid field function handle in chiPhysicsExportFieldFunctionToVTK";
    exit(EXIT_FAILURE);
  }

  ff->ExportToVTK(base_name,field_name);

  return 0;
}

//#############################################################################
/** Exports all the groups in a field function to VTK format.
 *
\param FFHandle int Global handle to the field function.
\param BaseName char Base name for the exported file.

\ingroup LuaFieldFunc
\author Jan*/
int chiExportFieldFunctionToVTKG(lua_State *L)
{
  int num_args = lua_gettop(L);
  if ((num_args < 2) or (num_args > 3))
    LuaPostArgAmountError("chiExportFieldFunctionToVTKG", 2, num_args);

  int ff_handle = lua_tonumber(L,1);
  const char* base_name = lua_tostring(L,2);
  const char* field_name = base_name;
  if (num_args == 3)
    field_name = lua_tostring(L,3);

  //======================================================= Getting solver
  chi_physics::FieldFunction* ff;
  try
  {
    ff = chi_physics_handler.fieldfunc_stack.at(ff_handle);
  }
  catch(const std::out_of_range& o)
  {
    chi_log.Log(LOG_ALLERROR)
      << "Invalid field function handle in chiPhysicsExportFieldFunctionToVTKG";
    exit(EXIT_FAILURE);
  }

  ff->ExportToVTKG(base_name,field_name);

  return 0;
}

//#############################################################################
/** Exports all the groups in a field function to VTK format and also
 * attached another field function.
 *
\param FFHandle int Global handle to the field function.
\param FFHandle2 int The Global handle to the secondary field function.
\param BaseName char Base name for the exported file.
\param FieldName char Optional field name.

\ingroup LuaFieldFunc
\author Jan*/
int chiExportMultiFieldFunctionToVTK(lua_State *L)
{
    if (lua_istable(L, 1) == 0)
    {
        chi_log.Log(LOG_0ERROR)
                << "chiExportMultiFieldFunctionToVTKG expected a lua table";
    }
    else
    {
        for (int i = 1; i < int(lua_rawlen(L, 1) + 1); ++i)
        {
            lua_pushstring(L, i);
            lua_gettable(L, 1);
            chi_log.Log(LOG_0) << lua_rawlen(L, 2);
            if (lua_istable(L, 2) == 0)
            {
                chi_log.Log(LOG_0ERROR)
                        << "chiExportMultiFieldFunctionToVTKG expected a lua table";
                exit(EXIT_FAILURE);
            }
            else
            {
                int num_args = lua_rawlen(L, 2);
                chi_log.Log(LOG_0) << num_args;
                if ((num_args < 2) or (num_args > 3))
                {
                    LuaPostArgAmountError("chiExportMultiFieldFunctionToVTKG", 3, num_args);
                    exit(EXIT_FAILURE);
                }
            }
            lua_pop(L, -1);
        }
    }

    int ff_handle;
    const char* field_name;
    const char* base_name;
    for (int i = 1; i < int(lua_rawlen(L, 1) + 1); ++i)
    {
        lua_pushnumber(L, i);
        lua_gettable(L, 1);
        int num_args = lua_rawlen(L, 2);
        for (int j = 1; j < num_args + 1; ++j)
        {
            lua_pushnumber(L, j);
            lua_gettable(L, 2);
            if (j == 1)
            {
                ff_handle = lua_tonumber(L, 3);
            }
            else
            {
                if (j == 3)
                    field_name = lua_tostring(L,3);
                else
                {
                    base_name = lua_tostring(L, 3);
                    field_name = base_name;
                }
            }

            lua_pop(L, -1);
        }
        lua_pop(L, -1);
    }

    //======================================================= Getting solver
    chi_log.Log(LOG_0) << ff_handle;
    chi_physics::FieldFunction* ff;
    try
    {
      ff = chi_physics_handler.fieldfunc_stack.at(ff_handle);
    }
    catch(const std::out_of_range& o)
    {
      chi_log.Log(LOG_ALLERROR)
        << "Invalid field function handle in chiPhysicsExportFieldFunctionToVTK";
      exit(EXIT_FAILURE);
    }

    ff->ExportToVTKG(base_name,field_name);

    return 0;
}
