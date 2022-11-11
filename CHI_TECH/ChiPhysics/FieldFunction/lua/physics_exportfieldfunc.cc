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
  if ((num_args < 2) or (num_args>3))
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
  if ((num_args < 2) or (num_args>3))
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
  int num_args = lua_gettop(L);
  if ((num_args < 3) or (num_args>4))
    LuaPostArgAmountError("chiExportMultiFieldFunctionToVTKG", 3, num_args);

  int ff_handle = lua_tonumber(L,1);
  int ff_handle_slave = lua_tonumber(L,2);
  const char* base_name = lua_tostring(L,3);
  const char* field_name = base_name;
  if (num_args == 4)
    field_name = lua_tostring(L,4);

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

  chi_physics::FieldFunction* ff_slave;
  try
  {
    ff_slave = chi_physics_handler.fieldfunc_stack.at(ff_handle_slave);
  }
  catch(const std::out_of_range& o)
  {
    chi_log.Log(LOG_ALLERROR)
      << "Invalid field function handle in chiPhysicsExportFieldFunctionToVTK";
    exit(EXIT_FAILURE);
  }

//  ff->ExportToVTKG(base_name,field_name);

  ff->ExportMultiToVTK(ff_slave,base_name,field_name);
  return 0;
}
