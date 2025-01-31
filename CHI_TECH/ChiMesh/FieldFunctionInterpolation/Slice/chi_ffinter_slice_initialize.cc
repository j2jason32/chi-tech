#include "chi_ffinter_slice.h"
#include "../../Cell/cell_slab.h"
#include "../../Cell/cell_polygon.h"
#include "../../Cell/cell_polyhedron.h"

#include <chi_log.h>

extern ChiLog chi_log;

/**Initializes the data structures necessary for interpolation. This is
 * independent of the physics and hence is a routine on its own.
 *
 * The first step of this initialization is to determine which cells
 * are intersected by this plane. For polyhedrons this is evaluated
 * tet-by-tet.
 *
 * The second step is find where face-edges are intersected. This will
 * effectively create intersection polygons.*/
void chi_mesh::FieldFunctionInterpolationSlice::
  Initialize()
{
  chi_log.Log(LOG_0VERBOSE_1) << "Initializing slice interpolator.";
  //================================================== Check grid available
  if (field_functions.size() == 0)
  {
    chi_log.Log(LOG_ALLERROR)
    << "Unassigned field function in slice field function interpolator.";
    exit(EXIT_FAILURE);
  } else
  {
    this->grid_view = field_functions[0]->grid;
  }

  //================================================== Find cells intersecting plane
  intersecting_cell_indices.clear();

  size_t num_local_cells = grid_view->local_cell_glob_indices.size();
  for (int lc=0; lc<num_local_cells; lc++)
  {
    int cell_glob_index = grid_view->local_cell_glob_indices[lc];
    auto cell = grid_view->cells[cell_glob_index];

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% SLAB
    if (cell->Type() == chi_mesh::CellType::SLAB)
    {
      chi_log.Log(LOG_0)
        << "FieldFunctionInterpolationSlice does not support 1D cells.";
      exit(EXIT_FAILURE);
    }

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% POLYGON
    if (cell->Type() == chi_mesh::CellType::POLYGON)
    {
      intersecting_cell_indices.push_back(cell_glob_index);
    }

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% POLYHEDRON
    else if (cell->Type() == chi_mesh::CellType::POLYHEDRON)
    {
      chi_mesh::CellPolyhedron* polyh_cell = (chi_mesh::CellPolyhedron*)cell;
      bool intersects = false;

      size_t num_faces = polyh_cell->faces.size();
      for (int f=0; f<num_faces; f++)
      {
        size_t num_edges = polyh_cell->faces[f]->edges.size();
        for (int e=0; e<num_edges; e++)
        {
          int v0_i = polyh_cell->faces[f]->edges[e][0];
          int v1_i = polyh_cell->faces[f]->edges[e][1];

          std::vector<chi_mesh::Vector*> tet_points;

          tet_points.push_back(grid_view->nodes[v0_i]);
          tet_points.push_back(grid_view->nodes[v1_i]);
          tet_points.push_back(&polyh_cell->faces[f]->face_centroid);
          tet_points.push_back(&polyh_cell->centroid);

          if (CheckPlaneTetIntersect(this->normal,this->point,&tet_points))
          {
            intersecting_cell_indices.push_back(cell_glob_index);
            intersects = true;
            break;
          }
        }//for e
        if (intersects) break;
      }//for f
    }//if polyh
  }//for local cell

  //================================================== Computing cell intersections
  size_t num_cut_cells = intersecting_cell_indices.size();
  for (int cc=0; cc<num_cut_cells; cc++)
  {
    int cell_glob_index = intersecting_cell_indices[cc];

    auto cell = grid_view->cells[cell_glob_index];

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% POLYGON
    if (cell->Type() == chi_mesh::CellType::POLYGON)
    {
      chi_mesh::CellPolygon* poly_cell = (chi_mesh::CellPolygon*)cell;

      //========================================= Initialize cell intersection
      //                                          data structure
      FFICellIntersection* cell_isds = new FFICellIntersection;
      cell_isds->cell_global_index = cell_glob_index;
      cell_intersections.push_back(cell_isds);

      //========================================= Loop over vertices
      for (int v=0; v<poly_cell->v_indices.size(); v++)
      {
        FFIFaceEdgeIntersection* face_isds =
          new FFIFaceEdgeIntersection;

        int v0gi = poly_cell->v_indices[v];

        face_isds->v0_g_index = v0gi;
        face_isds->v1_g_index = v0gi;

        face_isds->v0_dofindex_cell = v;
        face_isds->v1_dofindex_cell = v;

        face_isds->weights = std::pair<double,double>(0.5,0.5);
        face_isds->point   = *grid_view->nodes[v0gi];

        cell_isds->intersections.push_back(face_isds);
      }

      //========================================= Set intersection center
      cell_isds->intersection_centre = poly_cell->centroid;

      //========================================= Set straight 2D center
      // This is normally transformed for the 3D case
      cell_isds->intersection_2d_centre = cell_isds->intersection_centre;

      //========================================= Same for 2D points
      int num_points = cell_isds->intersections.size();
      for (int p=0; p<num_points; p++)
      {
        chi_mesh::Vector vref = cell_isds->intersections[p]->point-this->point;

        cell_isds->intersections[p]->point2d = vref;

        cfem_local_nodes_needed_unmapped.push_back(cell_isds->intersections[p]->v0_g_index);
        cfem_local_nodes_needed_unmapped.push_back(cell_isds->intersections[p]->v1_g_index);
        pwld_local_nodes_needed_unmapped.push_back(cell_isds->intersections[p]->v0_dofindex_cell);
        pwld_local_nodes_needed_unmapped.push_back(cell_isds->intersections[p]->v1_dofindex_cell);
        pwld_local_cells_needed_unmapped.push_back(cell_glob_index);
        pwld_local_cells_needed_unmapped.push_back(cell_glob_index);
      }
    }




    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% POLYHEDRON
    if (cell->Type() == chi_mesh::CellType::POLYHEDRON)
    {
      chi_mesh::CellPolyhedron* polyh_cell = (chi_mesh::CellPolyhedron*)cell;

      //========================================= Initialize cell intersection
      //                                          data structure
      FFICellIntersection* cell_isds = new FFICellIntersection;
      cell_isds->cell_global_index = cell_glob_index;
      cell_intersections.push_back(cell_isds);

      //========================================= Loop over faces
      size_t num_faces = polyh_cell->faces.size();
      for (int f=0; f<num_faces; f++)
      {
        //================================== Loop over edges
        size_t num_edges = polyh_cell->faces[f]->edges.size();
        for (int e=0; e<num_edges; e++)
        {
          int v0gi = polyh_cell->faces[f]->edges[e][0]; //global index v0
          int v1gi = polyh_cell->faces[f]->edges[e][1]; //global index v1

          chi_mesh::Vertex v0 = (*grid_view->nodes[v0gi]);
          chi_mesh::Vertex v1 = (*grid_view->nodes[v1gi]);

          chi_mesh::Vertex interstion_point;            //Placeholder
          std::pair<double,double> weights;

          //=========================== Check if intersects plane
          if (CheckPlaneLineIntersect(this->normal,this->point,
                                      v0,v1,interstion_point,
                                      weights))
          {
            //==================== Check for duplicate
            bool duplicate_found = false;
            for (int is=0; is<cell_isds->intersections.size(); is++)
            {
              FFIFaceEdgeIntersection* existing_face_isds =
                cell_isds->intersections[is];

              double dif = (existing_face_isds->point -
                            interstion_point).NormSquare();
              if (dif < 1.0e-6)
              {
                duplicate_found = true;
                break;
              }
            }

            //==================== No duplicate
            if (!duplicate_found)
            {
              FFIFaceEdgeIntersection* face_isds =
                new FFIFaceEdgeIntersection;

              //Find vertex 0 dof index
              face_isds->v0_g_index = v0gi;
              size_t num_dofs = polyh_cell->v_indices.size();
              for (int dof=0; dof<num_dofs; dof++)
              {
                if (polyh_cell->v_indices[dof] == v0gi)
                {
                  face_isds->v0_dofindex_cell = dof; break;
                }
              }

              //Find vertex 1 dof index
              face_isds->v1_g_index = v1gi;
              for (int dof=0; dof<num_dofs; dof++)
              {
                if (polyh_cell->v_indices[dof] == v1gi)
                {
                  face_isds->v1_dofindex_cell = dof; break;
                }
              }

              face_isds->weights = weights;
              face_isds->point = interstion_point;
              cell_isds->intersections.push_back(face_isds);
            }
          }//if intersecting
        }//for edge
      }//for face

      //====================================

      //==================================== Computing intersection centre
      size_t num_points = cell_isds->intersections.size();
      if (num_points>0)
      {
        for (int p=0; p<num_points; p++)
        {
          cell_isds->intersection_centre =
            cell_isds->intersection_centre +
            cell_isds->intersections[p]->point;
        }
        cell_isds->intersection_centre =
          cell_isds->intersection_centre/num_points;
      }
      else
      {
        chi_log.Log(LOG_ALLWARNING) << "No face intersections encountered "
                                       "for a cell that is indicated as being "
                                       "intersected. Slice FF interp.";
      }

      //==================================== Computing 2D transforms
      chi_mesh::Vector vref = cell_isds->intersection_centre - this->point;

      cell_isds->intersection_2d_centre.x = vref.Dot(tangent);
      cell_isds->intersection_2d_centre.y = vref.Dot(binorm);
      cell_isds->intersection_2d_centre.z = vref.Dot(normal);

      //==================================== Points
      std::vector<FFIFaceEdgeIntersection*> unsorted_points;
      for (int p=0; p<num_points; p++)
      {
        chi_mesh::Vector vref = cell_isds->intersections[p]->point-this->point;

        cell_isds->intersections[p]->point2d.x = vref.Dot(tangent);
        cell_isds->intersections[p]->point2d.y = vref.Dot(binorm);
        cell_isds->intersections[p]->point2d.z = vref.Dot(normal);

        unsorted_points.push_back(cell_isds->intersections[p]);
      }
      cell_isds->intersections.clear();

      //==================================== Sort points clockwise
      //The first point is retrieved from the unused stack.
      //Subsequent points are only added if they form a
      //convex line wrt the right hand rule.
      cell_isds->intersections.push_back(unsorted_points[0]);
      cfem_local_nodes_needed_unmapped.push_back(unsorted_points[0]->v0_g_index);
      cfem_local_nodes_needed_unmapped.push_back(unsorted_points[0]->v1_g_index);
      pwld_local_nodes_needed_unmapped.push_back(unsorted_points[0]->v0_dofindex_cell);
      pwld_local_nodes_needed_unmapped.push_back(unsorted_points[0]->v1_dofindex_cell);
      pwld_local_cells_needed_unmapped.push_back(cell_glob_index);
      pwld_local_cells_needed_unmapped.push_back(cell_glob_index);
      unsorted_points.erase(unsorted_points.begin());

      while (unsorted_points.size()>0)
      {
        for (int p=0; p<unsorted_points.size(); p++)
        {
          chi_mesh::Vector v1 = unsorted_points[p]->point2d -
                                cell_isds->intersections.back()->point2d;

          bool illegal_value = false;
          for (int pr=0; pr<unsorted_points.size(); pr++)
          {
            if (pr!=p)
            {
              chi_mesh::Vector vr = unsorted_points[pr]->point2d -
                                    unsorted_points[p]->point2d;

              if (vr.Cross(v1).z < 0.0)
              {
                illegal_value = true;
                break;
              }
            }//if not p
          }//for pr

          if (!illegal_value)
          {
            cell_isds->intersections.push_back(unsorted_points[p]);
            cfem_local_nodes_needed_unmapped.push_back(unsorted_points[p]->v0_g_index);
            cfem_local_nodes_needed_unmapped.push_back(unsorted_points[p]->v1_g_index);
            pwld_local_nodes_needed_unmapped.push_back(unsorted_points[p]->v0_dofindex_cell);
            pwld_local_nodes_needed_unmapped.push_back(unsorted_points[p]->v1_dofindex_cell);
            pwld_local_cells_needed_unmapped.push_back(cell_glob_index);
            pwld_local_cells_needed_unmapped.push_back(cell_glob_index);
            unsorted_points.erase(unsorted_points.begin()+p);
            break;
          }

        }//for p
      }

    }//if polyhedron
  }//for intersected cell

  //chi_log.Log(LOG_0) << "Finished initializing interpolator.";
}