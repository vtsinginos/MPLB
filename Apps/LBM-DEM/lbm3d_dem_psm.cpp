/**
 * Copyright 2019 United Kingdom Research and Innovation
 *
 * Authors: See AUTHORS
 *
 * Contact: [jianping.meng@stfc.ac.uk and/or jpmeng@gmail.com]
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice
 *    this list of conditions and the following disclaimer in the documentation
 *    and or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * ANDANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
*/

/** @brief An example main source code for coupling the DEM with the LBM with the PRATI scheme
 *  @author Chrysovalantis Tsigginos
 **/
#include <cmath>
#include <iostream>
#include <ostream>
#include <string>
#include "mplb.h"
#include "ops_seq_v2.h"
#include "mplb_dem.h"
#include "fpi.h"
#include "cavity3d_kernel.inc"

void SetInitialMacrosVars() {
    for (auto idBlock : g_Block()) {
        Block& block{idBlock.second};
        std::vector<int> iterRng;
        iterRng.assign(block.WholeRange().begin(), block.WholeRange().end());
        const int blockIdx{block.ID()};
        for (auto& idCompo : g_Components()) {
            const Component& compo{idCompo.second};
            const int rhoId{compo.macroVars.at(Variable_Rho).id};
            ops_par_loop(KerSetInitialMacroVars, "KerSetInitialMacroVars",
                         block.Get(), SpaceDim(), iterRng.data(),
                         ops_arg_dat(g_MacroVars().at(rhoId).at(blockIdx), 1,
                                     LOCALSTENCIL, "Real", OPS_RW),
                         ops_arg_dat(g_MacroVars().at(compo.uId).at(blockIdx),
                                     1, LOCALSTENCIL, "Real", OPS_RW),
                         ops_arg_dat(g_MacroVars().at(compo.vId).at(blockIdx),
                                     1, LOCALSTENCIL, "Real", OPS_RW),
                         ops_arg_dat(g_MacroVars().at(compo.wId).at(blockIdx),
                                     1, LOCALSTENCIL, "Real", OPS_RW),
                         ops_arg_dat(g_CoordinateXYZ()[blockIdx], SpaceDim(),
                                     LOCALSTENCIL, "Real", OPS_READ),
                         ops_arg_idx());
        }
    }
}


void UpdateMacroscopicBodyForce(const Real time) {}


void simulate() {


	//Define simulations and blocks
	ops_printf("Define simulation and blocks\n");
	std::string caseName{"LBM-DEM3D\n"};
	SizeType spaceDim{3};
	std::vector<int> blockIds{0};
	std::vector<std::string> blockNames{"Cavity"};
	std::vector<int> blockSize{33, 33, 33};
	Real meshSize(1./32);
	std::map<int, std::vector<Real>> startPos{{0, {0.0, 0.0, 0.0}}};
	DefineBlocks(blockIds, blockNames, blockSize, meshSize, startPos);


	//Define fluid components
	std::vector<std::string> componentNames{"Fluid"};
	std::vector<int> compoId{0};
	std::vector<std::string> lattNames {"d3q19"};
	std::vector<Real> tauRef{0.01};
	DefineComponents(componentNames, compoId, lattNames, tauRef);

	//Define macro-variables
    std::vector<VariableTypes> marcoVarTypes{Variable_Rho, Variable_U,
                                             Variable_V, Variable_W};
    std::vector<std::string> macroVarNames{"rho", "u", "v", "w"};
    std::vector<int> macroVarId{0, 1, 2, 3};
    std::vector<int> macroCompoId{0, 0, 0, 0};
    DefineMacroVars(marcoVarTypes, macroVarNames, macroVarId, macroCompoId);

    std::vector<CollisionType> collisionTypes{Collision_BGKIsothermal2nd};
    std::vector<int> collisionCompoId{0};
    DefineCollision(collisionTypes, collisionCompoId);

    std::vector<BodyForceType> bodyForceTypes{BodyForce_None};
    std::vector<SizeType> bodyForceCompoId{0};
    DefineBodyForce(bodyForceTypes, bodyForceCompoId);

    SchemeType scheme{Scheme_StreamCollision};
    DefineScheme(scheme);

    ops_printf("Defining fluid-particle interaction\n");
    Real cutoff = 0.01 * meshSize;
    std::string particleType{"spherical"};
    DefineBlockParticles(spaceDim, cutoff,  meshSize, particleType);

    //Define FSI-model
    std::vector<FSIType> fluidModel{1};
    std::vector<int> porosModelParams{1};//, 2, 2};
    std::vector<int> fsiComponent{0};
    Real force[spaceDim]={0.0, 0.0, 0.0};

    // Setting boundary conditions
    SizeType blockIndex{0};
    SizeType componentId{0};
    std::vector<VariableTypes> macroVarTypesatBoundary{Variable_U, Variable_V,
                                                       Variable_W};
    std::vector<Real> noSlipStationaryWall{0, 0, 0};
    // Left noSlipStationaryWall
    DefineBlockBoundary(blockIndex, componentId, BoundarySurface::Left,
                        BoundaryScheme::EQMDiffuseRefl, macroVarTypesatBoundary,
                        noSlipStationaryWall);
    // Right noSlipStationaryWall
    DefineBlockBoundary(blockIndex, componentId, BoundarySurface::Right,
                        BoundaryScheme::EQMDiffuseRefl, macroVarTypesatBoundary,
                        noSlipStationaryWall);
    // Top noslipMovingWall
    std::vector<Real> noSlipMovingWall{0.01, 0, 0};
    DefineBlockBoundary(blockIndex, componentId, BoundarySurface::Top,
                        BoundaryScheme::EQMDiffuseRefl, macroVarTypesatBoundary,
                        noSlipMovingWall);
    // bottom noSlipStationaryWall
    DefineBlockBoundary(blockIndex, componentId, BoundarySurface::Bottom,
                        BoundaryScheme::EQMDiffuseRefl, macroVarTypesatBoundary,
                        noSlipStationaryWall);
    // front noSlipStationaryWall
    DefineBlockBoundary(blockIndex, componentId, BoundarySurface::Front,
                        BoundaryScheme::EQMDiffuseRefl, macroVarTypesatBoundary,
                        noSlipStationaryWall);
    // back noSlipStationaryWall
    DefineBlockBoundary(blockIndex, componentId, BoundarySurface::Back,
                        BoundaryScheme::EQMDiffuseRefl, macroVarTypesatBoundary,
                        noSlipStationaryWall);

    std::vector<InitialType> initType{Initial_BGKFeq2nd};
    std::vector<SizeType> initalCompoId{0};
    DefineInitialCondition(initType,initalCompoId);


    Partition();

    SetInitialMacrosVars();
    SetTimeStep(meshSize / SoundSpeed());
    PreDefinedInitialCondition3D();

    //Initialize Particle Information
    InteractionData lbmDemData;
    bool muiflag = false;
    SetDemLbMParams(&lbmDemData, false, muiflag, 1e-7, 10000, 0.0, particleType);


    //SetMuiInterface
    if (lbmDemData.muiFlag) {
    	CreateMuiInterface(lbmDemData, fluidModel, cutoff);
    }

    SetupParticleBoxes(lbmDemData);

    //Setting up DEM-LBM
    SetupDEMLBM(lbmDemData);

    IterateFSI(lbmDemData, 1);

}

void simulate(const Configuration & config, const SizeType timeStep=0) {


}

int main(int argc, const char** argv) {
    // OPS initialisation
    ops_init(argc, argv, 4);
    double ct0, ct1, et0, et1;
    ops_timers(&ct0, &et0);
    // start a simulation by hard-coding
    std::string s1("readFile");
    std::string s2("restartFile");
    int iLoc, output = 0;

    if (argc <= 1) {
        simulate();
    }
    else {

    	for (int iArgs = 0; iArgs < argc; iArgs++) {

    		std::string argName (argv[iArgs]);
    		output = argName.compare(s1);
    		if ( argName.compare(s1) == 0) {
    			iLoc  = iArgs;
    			output = 1;
    			break;
    		}
    		else if (argName.compare(s2) == 0) {
    			iLoc = iArgs;
    			output = 2;
    			break;
    		}
    	}

    	if (output == 1) {
    		std::string configFileName(argv[iLoc+1]);
    		//ReadConfigurationDEM(configFileName); /* TODO ADD THE JSON FILE */
    		simulate(Config()); /*TODO ADD IT WITH JSON */
    	}
    	else if (output==2) {
    		std::string configFileName(argv[iLoc+1]);
    		//ReadConfigurationDEM(configFileName); /* TODO ADD THE JSON FILE */
    		const SizeType timeStep{static_cast<SizeType>(std::stoi(argv[iLoc+2]))};
    		 simulate(Config(),timeStep);
    	}
    	else {
    		ops_printf("WARNING: Option %d not supported\n", output);
    	}

    }


    ops_timers(&ct1, &et1);
    ops_printf("\nTotal Wall time %lf\n", et1 - et0);
    //Print OPS performance details to output stream
    ops_timing_output(std::cout);
    ops_exit();

}








