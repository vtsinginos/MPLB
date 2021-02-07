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

/** @brief An example main source code of stimulating 3D lid-driven cavity flow
 *  @author Jianping Meng
 **/
#include <cmath>
#include <iostream>
#include <ostream>
#include <string>
#include "boundary.h"
#include "evolution3d.h"
#include "flowfield.h"
#include "hilemms.h"
#include "model.h"
#include "ops_seq_v2.h"
#include "scheme.h"
#include "type.h"
#include "configuration.h"
#include "cavity3d.h"
//Provide macroscopic initial conditions
void SetInitialMacrosVars() {
    for (int blockIdx = 0; blockIdx < BlockNum(); blockIdx++) {
        int* iterRng = BlockIterRng(blockIdx, IterRngWhole());
        ops_par_loop(KerSetInitialMacroVars, "KerSetInitialMacroVars",
                     g_Block[blockIdx], SPACEDIM, iterRng,
                     ops_arg_dat(g_MacroVars[blockIdx], NUMMACROVAR,
                                 LOCALSTENCIL, "double", OPS_RW),
                     ops_arg_dat(g_CoordinateXYZ[blockIdx], SPACEDIM,
                                 LOCALSTENCIL, "double", OPS_READ),
                     ops_arg_idx());
    }
}
//Provide macroscopic body-force term
void UpdateMacroscopicBodyForce(const Real time) {}

void simulate() {

    std::string caseName{"3D_lid_Driven_cavity"};
    SizeType spaceDim{3};
    DefineCase(caseName, spaceDim);
    SizeType blockNum{1};
    std::vector<SizeType> blockSize{33, 33, 33};
    Real meshSize{1. / 32};
    std::vector<Real> startPos{0.0, 0.0, 0.0};
    DefineBlocks(blockNum, blockSize, meshSize, startPos);

    std::vector<std::string> compoNames{"Fluid"};
    std::vector<SizeType> compoid{0};
    std::vector<std::string> lattNames{"d3q19"};
    DefineComponents(compoNames, compoid, lattNames);

    std::vector<VariableTypes> marcoVarTypes{Variable_Rho, Variable_U,
                                             Variable_V, Variable_W};
    std::vector<std::string> macroVarNames{"rho", "u", "v", "w"};
    std::vector<SizeType> macroVarId{0, 1, 2, 3};
    std::vector<SizeType> macroCompoId{0, 0, 0, 0};
    DefineMacroVars(marcoVarTypes, macroVarNames, macroVarId, macroCompoId);

    std::vector<CollisionType> collisionTypes{Collision_BGKIsothermal2nd};
    std::vector<SizeType> collisionCompoId{0};
    DefineCollision(collisionTypes, collisionCompoId);

    std::vector<BodyForceType> bodyForceTypes{BodyForce_None};
    std::vector<SizeType> bodyForceCompoId{0};
    DefineBodyForce(bodyForceTypes, bodyForceCompoId);

    SchemeType scheme{Scheme_StreamCollision};
    DefineScheme(scheme);

    // Setting boundary conditions
    SizeType blockIndex{0};
    SizeType componentId{0};
    std::vector<VariableTypes> macroVarTypesatBoundary{Variable_U, Variable_V,
                                                       Variable_W};
    std::vector<Real> noSlipStationaryWall{0, 0, 0};
    // Left noSlipStationaryWall
    DefineBlockBoundary(blockIndex, componentId, BoundarySurface_Left,
                        BoundaryScheme::EQMDiffuseRefl, macroVarTypesatBoundary,
                        noSlipStationaryWall);
    // Right noSlipStationaryWall
    DefineBlockBoundary(blockIndex, componentId, BoundarySurface_Right,
                        BoundaryScheme::EQMDiffuseRefl, macroVarTypesatBoundary,
                        noSlipStationaryWall);
    // Top noslipMovingWall
    std::vector<Real> noSlipMovingWall{0.01, 0, 0};
    DefineBlockBoundary(blockIndex, componentId, BoundarySurface_Top,
                        BoundaryScheme::EQMDiffuseRefl, macroVarTypesatBoundary,
                        noSlipMovingWall);
    // bottom noSlipStationaryWall
    DefineBlockBoundary(blockIndex, componentId, BoundarySurface_Bottom,
                        BoundaryScheme::EQMDiffuseRefl, macroVarTypesatBoundary,
                        noSlipStationaryWall);
    // front noSlipStationaryWall
    DefineBlockBoundary(blockIndex, componentId, BoundarySurface_Front,
                        BoundaryScheme::EQMDiffuseRefl, macroVarTypesatBoundary,
                        noSlipStationaryWall);
    // back noSlipStationaryWall
    DefineBlockBoundary(blockIndex, componentId, BoundarySurface_Back,
                        BoundaryScheme::EQMDiffuseRefl, macroVarTypesatBoundary,
                        noSlipStationaryWall);

    std::vector<InitialType> initType{Initial_BGKFeq2nd};
    std::vector<SizeType> initalCompoId{0};
    DefineInitialCondition(initType,initalCompoId);
    Partition();
    SetInitialMacrosVars();
    PreDefinedInitialCondition3D();
    std::vector<Real> tauRef{0.01};
    SetTauRef(tauRef);
    SetTimeStep(meshSize / SoundSpeed());

    const Real convergenceCriteria{1E-7};
    const SizeType checkPeriod{1000};
    Iterate(StreamCollision3D,convergenceCriteria, checkPeriod);
}

void simulate(const Configuration & config, const SizeType timeStep=0) {
    DefineCase(config.caseName, config.spaceDim);
    DefineBlocks(config.blockNum, config.blockSize, config.meshSize,
                        config.startPos);
    if (timeStep == 0) {
        DefineComponents(config.compoNames, config.compoIds, config.lattNames);
        DefineMacroVars(config.macroVarTypes, config.macroVarNames,
                        config.macroVarIds, config.macroCompoIds);
    } else {
        // restart from a time step
        DefineComponents(config.compoNames, config.compoIds, config.lattNames,
                         timeStep);
        DefineMacroVars(config.macroVarTypes, config.macroVarNames,
                        config.macroVarIds, config.macroCompoIds,timeStep);
    }

    DefineCollision(config.CollisionTypes, config.CollisionCompoIds);
    DefineBodyForce(config.bodyForceTypes, config.bodyForceCompoIds);
    DefineScheme(config.schemeType);
    DefineInitialCondition(config.initialTypes,config.initialConditionCompoId);
    for (auto& bcConfig : config.blockBoundaryConfig) {
        DefineBlockBoundary(bcConfig.blockIndex, bcConfig.componentID,
                            bcConfig.boundarySurface, bcConfig.boundaryScheme,
                            bcConfig.macroVarTypesatBoundary,
                            bcConfig.givenVars, bcConfig.boundaryType);
    }
    Partition();
    if (timeStep == 0) {
        SetInitialMacrosVars();
        PreDefinedInitialCondition3D();
    } else{
        //Help function for restart a steady simulation
        //Mainly make the residual calculation correct at first iteration.
        RestartMacroVars4SteadySim();
    }

    SetTauRef(config.tauRef);
    SetTimeStep(config.meshSize / SoundSpeed());
    if (config.transient){
        Iterate(config.timeSteps, config.checkPeriod,timeStep);
    } else{
        Iterate(config.convergenceCriteria, config.checkPeriod,timeStep);
    }
}

int main(int argc, const char** argv) {
    // OPS initialisation
    ops_init(argc, argv, 1);
    double ct0, ct1, et0, et1;
    ops_timers(&ct0, &et0);
    // start a simulation by hard-coding
    if (argc <= 1) {
        simulate();
    }
    // start a new simulaton from a configuration file
    if (argc>1 && argc <=2){
        std::string configFileName(argv[1]);
        ReadConfiguration(configFileName);
        simulate(Config());
    }
    // restart from the time step specified by argv[2]
    if (argc>2 && argc <=3){
        std::string configFileName(argv[1]);
        ReadConfiguration(configFileName);
        const SizeType timeStep{static_cast<SizeType>(std::stoi(argv[2]))};
        simulate(Config(),timeStep);
    }

    ops_timers(&ct1, &et1);
    ops_printf("\nTotal Wall time %lf\n", et1 - et0);
    //Print OPS performance details to output stream
    ops_timing_output(std::cout);
    ops_exit();
}