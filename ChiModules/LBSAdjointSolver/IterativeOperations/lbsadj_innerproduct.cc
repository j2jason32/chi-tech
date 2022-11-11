#include "LBSAdjointSolver/lbsadj_solver.h"

#include "ChiMath/SpatialDiscretization/FiniteElement/spatial_discretization_FE.h"

//###################################################################
/**Computes the inner product of the flux and the material source.*/
double lbs_adjoint::AdjointSolver::ComputeInnerProduct()
{
  double local_integral = 0.0;

  auto pwl =
      std::dynamic_pointer_cast<chi_math::SpatialDiscretization_FE>(discretization);

  //============================================= Material sources
  for (const auto& cell : grid->local_cells)
  {
    if (matid_to_src_map.count(cell.material_id) == 0) continue; //Skip if no src

    const auto& transport_view = cell_transport_views[cell.local_id];
    const auto& source = matid_to_src_map[cell.material_id];
    const auto& fe_values = pwl->GetUnitIntegrals(cell);

    for (const auto& group : groups)
    {
      const int g = group.id;
      const double Q = source->source_value_g[g];

      if (Q > 0.0)
      {
        const int num_nodes = transport_view.NumNodes();
        for (int i = 0; i < num_nodes; ++i)
        {
          const size_t dof_map = transport_view.MapDOF(i, 0, g); //unknown map

          const double phi = phi_old_local[dof_map];

          local_integral += Q * phi * fe_values.IntV_shapeI(i);
        }//for node
      }//check source value >0
    }//for group
  }//for cell

  //============================================= Point sources
  for (const auto& point_source : point_sources)
  {
    const auto& info_list = point_source.ContainingCellsInfo();
    for (const auto& info : info_list)
    {
      const auto& cell = grid->local_cells[info.cell_local_id];
      const auto& transport_view = cell_transport_views[cell.local_id];
      const auto& source_strength = point_source.Strength();
      const auto& shape_values = info.shape_values;
      const auto& fe_values = pwl->GetUnitIntegrals(cell);

      for (const auto& group : groups)
      {
        const int g = group.id;
        const double S = source_strength[g] * info.volume_weight;

        if (S > 0.0)
        {
          const int num_nodes = transport_view.NumNodes();
          for (int i = 0; i < num_nodes; ++i)
          {
            const size_t dof_map = transport_view.MapDOF(i, 0, g); //unknown map

            const double phi_i = phi_old_local[dof_map];

            local_integral += S * phi_i * shape_values[i];
          }//for node
        }//check source value >0
      }//for group
    }//for cell
  }//for point source

  //============================================= Incident Flux Case
  for (const auto& groupset : groupsets)
  {
    const auto& quadrature = groupset.quadrature;
    const size_t num_dirs = quadrature->omegas.size();

    const auto& d2m = groupset.quadrature->GetDiscreteToMomentOperator();
    const auto& m2d = groupset.quadrature->GetMomentToDiscreteOperator();

    for (const auto& group : groupset.groups)
    {
      const auto g = group.id;
      for (size_t n=0; n<num_dirs; ++n)
      {
        const auto& omega = quadrature->omegas[n];
        const auto& w     = quadrature->weights[n];

        for (uint64_t cell_local_id : cells_on_bndry)
        {
          const auto& cell = grid->local_cells[cell_local_id];
          const auto& transport_view = cell_transport_views[cell.local_id];
          const auto& fe_values = pwl->GetUnitIntegrals(cell);
          const auto& S = fe_values.GetIntS_shapeI();

          for (size_t f=0; f<cell.faces.size(); ++f)
          {
            const auto& face = cell.faces[f];
            if (face.has_neighbor) continue; //skip internal faces

            const auto& S_f = S[f];
            const double mu_f = omega.Dot(face.normal);

            // const double q_surf = (mu_f>0.0)? 0.0 : 1.0;
            double q_surf = 0.0;
            if (mu_f < 0)
            {
                const auto& boundary = *sweep_boundaries.at(face.neighbor_id);
                if (boundary.Type() == chi_mesh::sweep_management::BoundaryType::INCIDENT_HOMOGENOUS)
                    q_surf = boundary.boundary_flux.at(g);
            }


            for (size_t fi=0; fi<face.vertex_ids.size(); ++fi)
            {
              const auto i = fe_values.FaceDofMapping(f, fi);
              const auto dof_map = transport_view.MapDOF(i, 0, g);
              double phi_star_gin = phi_old_local[dof_map];
              phi_star_gin *= m2d[0][n];

              //Make expansion of phi_star
            //   double psi_star_gin = 0.0;
            //   for (size_t m=0; m<num_moments; ++m)
            //   {
            //     const auto dof_map = transport_view.MapDOF(i, m, g);
            //     const double phi_star_m = phi_old_local[dof_map];

            //     psi_star_gin += phi_star_m;
            //   }//for m

              local_integral -= w * q_surf * phi_star_gin * mu_f * S_f[i];  
            }//for fi
          }//for face
        }//for cell
      }//for dir_n
    }//for group
  }//for groupset

  double global_integral = 0.0;

  MPI_Allreduce(&local_integral,     //sendbuf
                &global_integral,    //recvbuf
                1, MPI_DOUBLE,       //count, datatype
                MPI_SUM,             //op
                MPI_COMM_WORLD);     //comm

  return global_integral;
}