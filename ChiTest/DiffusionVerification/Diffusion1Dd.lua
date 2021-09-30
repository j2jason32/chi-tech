print("############################################### LuaTest")
--dofile(CHI_LIBRARY)



--############################################### Setup mesh
chiMeshHandlerCreate()

mesh={}
N=50
L=2.0
xmin = -1.0
dx = L/N
for i=1,(N+1) do
    k=i-1
    mesh[i] = xmin + k*dx
end
line_mesh = chiLineMeshCreateFromArray(mesh)


region1 = chiRegionCreate()
chiRegionAddLineBoundary(region1,line_mesh);


--############################################### Create meshers
chiSurfaceMesherCreate(SURFACEMESHER_PREDEFINED);
chiVolumeMesherCreate(VOLUMEMESHER_LINEMESH1D);


--############################################### Execute meshing
chiSurfaceMesherExecute();
chiVolumeMesherExecute();

--############################################### Set Material IDs
vol0 = chiLogicalVolumeCreate(RPP,-1000,1000,-1000,1000,-1000,1000)
chiVolumeMesherSetProperty(MATID_FROMLOGICAL,vol0,0)


--############################################### Add materials
materials = {}
materials[0] = chiPhysicsAddMaterial("Test Material");

sigt = 0.5
D = 1/3/sigt
chiPhysicsMaterialAddProperty(materials[0],SCALAR_VALUE,"DiffCoeff")
chiPhysicsMaterialSetProperty(materials[0],"DiffCoeff",SINGLE_VALUE,D)

chiPhysicsMaterialAddProperty(materials[0],SCALAR_VALUE,"Source")
chiPhysicsMaterialSetProperty(materials[0],"Source",SINGLE_VALUE,0)

chiPhysicsMaterialAddProperty(materials[0],SCALAR_VALUE,"SigmaA")
chiPhysicsMaterialSetProperty(materials[0],"SigmaA",SINGLE_VALUE,sigt)



--############################################### Setup Physics
phys1 = chiDiffusionCreateSolver();
chiSolverAddRegion(phys1,region1)
fftemp = chiSolverAddFieldFunction(phys1,"Temperature")
chiDiffusionSetProperty(phys1,DISCRETIZATION_METHOD,PWLC);
chiDiffusionSetProperty(phys1,RESIDUAL_TOL,1.0e-4)



--############################################### Initialize Solver
chiDiffusionInitialize(phys1)
--############################################### Set boundary conditions
chiDiffusionSetProperty(phys1,"boundary_type",0,"dirichlet",0.5)
chiDiffusionSetProperty(phys1,"boundary_type",1,"vacuum")

chiDiffusionExecute(phys1)
line0 = chiFFInterpolationCreate(LINE)
chiFFInterpolationSetProperty(line0,LINE_FIRSTPOINT,0.0+xmin,0.0,0.0)
chiFFInterpolationSetProperty(line0,LINE_SECONDPOINT, 2.0+xmin,0.0,0.0)
chiFFInterpolationSetProperty(line0,LINE_NUMBEROFPOINTS, 50)
chiFFInterpolationSetProperty(line0,ADD_FIELDFUNCTION,fftemp)

chiFFInterpolationInitialize(line0)
chiFFInterpolationExecute(line0)
chiFFInterpolationExportPython(line0)
--
local handle = io.popen("python ZLFFI00.py")