/**
 * Copyright 2019 United Kingdom Research and Innovation
 *
 * Authors: See AUTHORS
 *
 * Contact: [jianping.meng@stfc.ac.uk and/or jpmeng@gmail.com]
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the *following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,    *    this list of conditions and the following disclaimer.
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

/*! @brief   Wrap functions for main evolution cycle.
 * @author  Jianping Meng
 * @details Define wrap functions for implementing the main evolution
 * cycle
 */

#include "evolution.h"
#include "model.h"
#include "hilemms.h"
/*
 * In the following routines, there are some variables are defined
 * for the convenience of the translator which may not be able to
 * understand a function parameter in the ops_par_loop call
 * Even though, a variable rather than a numerical literacy will need
 * some modifications in the Python translator.
 */

#ifdef OPS_2D
void UpdateTau() {
    for (int blockIndex = 0; blockIndex < BlockNum(); blockIndex++) {
        int* iterRng = BlockIterRng(blockIndex, IterRngWhole());
        ops_par_loop(KerCalcTau, "KerCalcTau", g_Block[blockIndex], SPACEDIM,
                     iterRng,
                     ops_arg_dat(g_NodeType[blockIndex], NUMCOMPONENTS,
                                 LOCALSTENCIL, "int", OPS_READ),
                     ops_arg_gbl(TauRef(), NUMCOMPONENTS, "double", OPS_READ),
                     ops_arg_dat(g_MacroVars[blockIndex], NUMMACROVAR,
                                 LOCALSTENCIL, "double", OPS_READ),
                     ops_arg_dat(g_Tau[blockIndex], NUMCOMPONENTS, LOCALSTENCIL,
                                 "double", OPS_RW));
    }
}

void Collision() {
    for (int blockIndex = 0; blockIndex < BlockNum(); blockIndex++) {
        int* iterRng = BlockIterRng(blockIndex, IterRngWhole());
        ops_par_loop(KerCollide, "KerCollide", g_Block[blockIndex], SPACEDIM,
                     iterRng, ops_arg_gbl(pTimeStep(), 1, "double", OPS_READ),
                     ops_arg_dat(g_NodeType[blockIndex], NUMCOMPONENTS,
                                 LOCALSTENCIL, "int", OPS_READ),
                     ops_arg_dat(g_f[blockIndex], NUMXI, LOCALSTENCIL, "double",
                                 OPS_READ),
                     ops_arg_dat(g_feq[blockIndex], NUMXI, LOCALSTENCIL,
                                 "double", OPS_READ),
                     ops_arg_dat(g_Tau[blockIndex], NUMCOMPONENTS, LOCALSTENCIL,
                                 "double", OPS_READ),
                     ops_arg_dat(g_Bodyforce[blockIndex], NUMXI, LOCALSTENCIL,
                                 "double", OPS_READ),
                     ops_arg_dat(g_fStage[blockIndex], NUMXI, LOCALSTENCIL,
                                 "double", OPS_WRITE));
    }
}

void Stream() {
    for (int blockIndex = 0; blockIndex < BlockNum(); blockIndex++) {
        int* iterRng = BlockIterRng(blockIndex, IterRngWhole());
        ops_par_loop(KerStream, "KerStream", g_Block[blockIndex], SPACEDIM,
                     iterRng,
                     ops_arg_dat(g_NodeType[blockIndex], NUMCOMPONENTS,
                                 LOCALSTENCIL, "int", OPS_READ),
                     ops_arg_dat(g_GeometryProperty[blockIndex], 1,
                                 LOCALSTENCIL, "int", OPS_READ),
                     ops_arg_dat(g_fStage[blockIndex], NUMXI,
                                 ONEPTLATTICESTENCIL, "double", OPS_READ),
                     ops_arg_dat(g_f[blockIndex], NUMXI, LOCALSTENCIL, "double",
                                 OPS_RW));
    }
}

void UpdateMacroVars() {
    for (int blockIndex = 0; blockIndex < BlockNum(); blockIndex++) {
        int* iterRng = BlockIterRng(blockIndex, IterRngWhole());
        ops_par_loop(KerCalcMacroVars, "KerCalcMacroVars", g_Block[blockIndex],
                     SPACEDIM, iterRng,
                     ops_arg_dat(g_NodeType[blockIndex], NUMCOMPONENTS,
                                 LOCALSTENCIL, "int", OPS_READ),
                     ops_arg_dat(g_f[blockIndex], NUMXI, LOCALSTENCIL, "double",
                                 OPS_READ),
                     ops_arg_dat(g_MacroVars[blockIndex], NUMMACROVAR,
                                 LOCALSTENCIL, "double", OPS_RW));
    }
}

void UpdateFeqandBodyforce() {
    for (int blockIndex = 0; blockIndex < BlockNum(); blockIndex++) {
        int* iterRng = BlockIterRng(blockIndex, IterRngWhole());
        ops_par_loop(KerCalcFeq, "KerCalcPolyFeq", g_Block[blockIndex],
                     SPACEDIM, iterRng,
                     ops_arg_dat(g_NodeType[blockIndex], NUMCOMPONENTS,
                                 LOCALSTENCIL, "int", OPS_READ),
                     ops_arg_dat(g_MacroVars[blockIndex], NUMMACROVAR,
                                 LOCALSTENCIL, "double", OPS_READ),
                     ops_arg_dat(g_feq[blockIndex], NUMXI, LOCALSTENCIL,
                                 "double", OPS_RW));
        // force term to be added
    }
}

void TreatDomainBoundary(const int blockIndex, const int componentID,
                         const Real* givenVars, int* range,
                         const VertexTypes boundaryType)

// void TreatDomainBoundary(const Real* givenVars, int* range,const VertexTypes
// boundaryType)
{
    // for (int blockIdx = 0; blockIdx < BlockNum(); blockIdx++) {
    int blockIdx;
    blockIdx =
        blockIndex;  // This way will require minimum changes in the MPLB code.

    switch (boundaryType) {
        case Vertex_NoneqExtrapol: {
            // ops_par_loop(
            //     KerCutCellNonEqExtrapol, "KerCutCellNonEqExtrapol",
            //     g_Block[blockIdx], SPACEDIM, range,
            //     ops_arg_gbl(givenVars, NUMMACROVAR, "double", OPS_READ),
            //     ops_arg_dat(g_NodeType[blockIdx], 1, LOCALSTENCIL, "int",
            //                 OPS_READ),
            //     ops_arg_dat(g_GeometryProperty[blockIdx], 1, LOCALSTENCIL,
            //                 "int", OPS_READ),
            //     ops_arg_dat(g_MacroVars[blockIdx], NUMMACROVAR,
            //                 ONEPTLATTICESTENCIL, "double", OPS_READ),
            //     ops_arg_dat(g_feq[blockIdx], NUMXI, ONEPTLATTICESTENCIL,
            //                 "double", OPS_RW),
            //     ops_arg_dat(g_f[blockIdx], NUMXI, ONEPTLATTICESTENCIL,
            //                 "double", OPS_RW));
            //     //ops_arg_gbl(componentID, 1, "int", OPS_READ));
        } break;
        case Vertex_ExtrapolPressure1ST: {
            ops_par_loop(
                KerCutCellExtrapolPressure1ST, "KerCutCellExtrapolPressure1ST",
                g_Block[blockIdx], SPACEDIM, range,
                ops_arg_gbl(givenVars, NUMMACROVAR, "double", OPS_READ),
                ops_arg_dat(g_NodeType[blockIdx], NUMCOMPONENTS,
                            ONEPTREGULARSTENCIL, "int", OPS_READ),
                ops_arg_dat(g_GeometryProperty[blockIdx], 1, LOCALSTENCIL,
                            "int", OPS_READ),
                ops_arg_dat(g_f[blockIdx], NUMXI, ONEPTREGULARSTENCIL, "double",
                            OPS_RW));
        } break;
        case Vertex_ExtrapolPressure2ND: {
            // ops_par_loop(
            //     KerCutCellExtrapolPressure2ND,
            //     "KerCutCellExtrapolPressure2ND", g_Block[blockIdx],
            //     SPACEDIM, range,
            //     ops_arg_gbl(givenVars, NUMMACROVAR, "double", OPS_READ),
            //     ops_arg_dat(g_NodeType[blockIdx], 1, ONEPTREGULARSTENCIL,
            //                 "int", OPS_READ),
            //     ops_arg_dat(g_GeometryProperty[blockIdx], 1, LOCALSTENCIL,
            //                 "int", OPS_READ),
            //     ops_arg_dat(g_f[blockIdx], NUMXI, TWOPTREGULARSTENCIL,
            //                 "double", OPS_RW));
        } break;
        case Vertex_NonEqExtrapolPressure: {
            // ops_par_loop(
            //     KerCutCellNonEqExtrapolPressure,
            //     "KerCutCellNonEqExtrapolPressure", g_Block[blockIdx],
            //     SPACEDIM, range,
            //     ops_arg_gbl(givenVars, NUMMACROVAR, "double", OPS_READ),
            //     ops_arg_dat(g_NodeType[blockIdx], 1, LOCALSTENCIL, "int",
            //                 OPS_READ),
            //     ops_arg_dat(g_GeometryProperty[blockIdx], 1, LOCALSTENCIL,
            //                 "int", OPS_READ),
            //     ops_arg_dat(g_MacroVars[blockIdx], NUMMACROVAR,
            //                 ONEPTLATTICESTENCIL, "double", OPS_READ),
            //     ops_arg_dat(g_feq[blockIdx], NUMXI, ONEPTLATTICESTENCIL,
            //                 "double", OPS_RW),
            //     ops_arg_dat(g_f[blockIdx], NUMXI, ONEPTLATTICESTENCIL,
            //                 "double", OPS_RW));
        } break;
        case Vertex_BounceBackWall: {
            // ops_par_loop(KerCutCellBounceBack, "KerCutCellBounceBack",
            //              g_Block[blockIdx], SPACEDIM, range,
            //              ops_arg_dat(g_NodeType[blockIdx], 1, LOCALSTENCIL,
            //                          "int", OPS_READ),
            //              ops_arg_dat(g_GeometryProperty[blockIdx], 1,
            //                          LOCALSTENCIL, "int", OPS_READ),
            //              ops_arg_dat(g_f[blockIdx], NUMXI, LOCALSTENCIL,
            //                          "double", OPS_RW));
        } break;
        case Vertex_ZouHeVelocity: {
            // ops_par_loop(
            //     KerCutCellZouHeVelocity, "KerCutCellZouHeVelocity,",
            //     g_Block[blockIdx], SPACEDIM, range,
            //     ops_arg_gbl(givenVars, NUMMACROVAR, "double", OPS_READ),
            //     ops_arg_dat(g_NodeType[blockIdx], 1, LOCALSTENCIL, "int",
            //                 OPS_READ),
            //     ops_arg_dat(g_GeometryProperty[blockIdx], 1, LOCALSTENCIL,
            //                 "int", OPS_READ),
            //     ops_arg_dat(g_MacroVars[blockIdx], NUMMACROVAR,
            //                 ONEPTLATTICESTENCIL, "double", OPS_READ),
            //     ops_arg_dat(g_f[blockIdx], NUMXI, ONEPTLATTICESTENCIL,
            //                 "double", OPS_RW));
        } break;
        case Vertex_KineticDiffuseWall: {
            // ops_par_loop(
            //     KerCutCellKinetic, "KerCutCellKinetic", g_Block[blockIdx],
            //     SPACEDIM, range,
            //     ops_arg_gbl(givenVars, NUMMACROVAR, "double", OPS_READ),
            //     ops_arg_dat(g_NodeType[blockIdx], 1, LOCALSTENCIL, "int",
            //                 OPS_READ),
            //     ops_arg_dat(g_GeometryProperty[blockIdx], 1, LOCALSTENCIL,
            //                 "int", OPS_READ),
            //     ops_arg_dat(g_f[blockIdx], NUMXI, LOCALSTENCIL, "double",
            //                 OPS_RW));
        }
        case Vertex_EQMDiffuseRefl: {
            ops_par_loop(
                KerCutCellEQMDiffuseRefl, "KerCutCellEQMDiffuseRefl",
                g_Block[blockIdx], SPACEDIM, range,
                ops_arg_gbl(givenVars, NUMMACROVAR, "double", OPS_READ),
                ops_arg_dat(g_NodeType[blockIdx], NUMCOMPONENTS, LOCALSTENCIL,
                            "int", OPS_READ),
                ops_arg_dat(g_GeometryProperty[blockIdx], 1, LOCALSTENCIL,
                            "int", OPS_READ),
                ops_arg_dat(g_f[blockIdx], NUMXI, LOCALSTENCIL, "double",
                            OPS_RW),
                ops_arg_gbl(&componentID, 1, "int", OPS_READ));
        } break;
        case Vertex_FreeFlux: {
            // ops_par_loop(KerCutCellZeroFlux, "KerCutCellZeroFlux",
            //              g_Block[blockIdx], SPACEDIM, range,
            //              ops_arg_dat(g_NodeType[blockIdx], 1, LOCALSTENCIL,
            //                          "int", OPS_READ),
            //              ops_arg_dat(g_GeometryProperty[blockIdx], 1,
            //                          LOCALSTENCIL, "int", OPS_READ),
            //              ops_arg_dat(g_f[blockIdx], NUMXI, LOCALSTENCIL,
            //                          "double", OPS_RW));
        } break;
        case Vertex_Periodic: {
            ops_par_loop(KerCutCellPeriodic, "KerCutCellPeriodic",
                         g_Block[blockIdx], SPACEDIM, range,
                         ops_arg_dat(g_NodeType[blockIdx], NUMCOMPONENTS,
                                     LOCALSTENCIL, "int", OPS_READ),
                         ops_arg_dat(g_GeometryProperty[blockIdx], 1,
                                     LOCALSTENCIL, "int", OPS_READ),
                         ops_arg_dat(g_f[blockIdx], NUMXI, LOCALSTENCIL,
                                     "double", OPS_RW));
        } break;
        default:
            break;
    }
    //}
}

void TreatEmbeddedBoundary() {
    for (int blockIdx = 0; blockIdx < BlockNum(); blockIdx++) {
        int* iterRng = BlockIterRng(blockIdx, IterRngBulk());
        ops_par_loop(
            KerCutCellEmbeddedBoundary, "KerCutCellImmersedBoundary",
            g_Block[blockIdx], SPACEDIM, iterRng,
            ops_arg_dat(g_NodeType[blockIdx], NUMCOMPONENTS, LOCALSTENCIL,
                        "int", OPS_READ),
            ops_arg_dat(g_GeometryProperty[blockIdx], 1, LOCALSTENCIL, "int",
                        OPS_READ),
            ops_arg_dat(g_f[blockIdx], NUMXI, LOCALSTENCIL, "double", OPS_RW));
    }
}

/*
void ImplementBoundary() {
    // TreatEmbeddedBoundary();
    // Real givenInletVars[]{1.00005, 0, 0};
    int* inletRng = BlockIterRng(0, IterRngImin());
    Real givenInletVars[]{1, 0.05, 0};  // Input Parameters
    TreatDomainBoundary(givenInletVars, inletRng, Vertex_FreeFlux);
    int* outletRng = BlockIterRng(0, IterRngImax());
    Real givenOutletVars[] = {1, 0, 0};  // Input Parameters
    TreatDomainBoundary(givenOutletVars, outletRng, Vertex_FreeFlux);
    int* topRng = BlockIterRng(0, IterRngJmax());
    // Real givenTopWallBoundaryVars[]{1, 0, 0};
    Real givenTopWallBoundaryVars[]{1, 0, 0};  // Input Parameters
    TreatDomainBoundary(givenTopWallBoundaryVars, topRng, Vertex_FreeFlux);
    int* bottomRng = BlockIterRng(0, IterRngJmin());
    Real givenBotWallBoundaryVars[]{1, 0, 0};  // Input Parameters
    TreatDomainBoundary(givenBotWallBoundaryVars, bottomRng, Vertex_FreeFlux);
}
*/

void InitialiseSolution() {
    UpdateFeqandBodyforce();
    for (int blockIndex = 0; blockIndex < BlockNum(); blockIndex++) {
        int* iterRng = BlockIterRng(blockIndex, IterRngWhole());
        const Real zero = 0;
        ops_par_loop(
            KerSetfFixValue, "KerSetfFixValue", g_Block[blockIndex], SPACEDIM,
            iterRng, ops_arg_gbl(&zero, 1, "double", OPS_READ),
            ops_arg_dat(g_Bodyforce[0], NUMXI, LOCALSTENCIL, "double", OPS_RW));
    }
    CopyDistribution(g_feq, g_f);
}

void CopyDistribution(const ops_dat* fSrc, ops_dat* fDest) {
    for (int blockIndex = 0; blockIndex < BlockNum(); blockIndex++) {
        int* iterRng = BlockIterRng(blockIndex, IterRngWhole());
        ops_par_loop(KerCopyf, "KerCopyf", g_Block[blockIndex], SPACEDIM,
                     iterRng,
                     ops_arg_dat(fSrc[blockIndex], NUMXI, LOCALSTENCIL,
                                 "double", OPS_READ),
                     ops_arg_dat(fDest[blockIndex], NUMXI, LOCALSTENCIL,
                                 "double", OPS_WRITE));
    }
}

void CalcTotalMass(double* totalMass) {
    ops_reduction massHandle =
        ops_decl_reduction_handle(sizeof(double), "double", "massHandle");
    for (int blockIdx = 0; blockIdx < BlockNum(); blockIdx++) {
        int* iterRng = BlockIterRng(blockIdx, IterRngWhole());
        ops_par_loop(KerCalcSumofDensity, "KerCalcSumofDensity",
                     g_Block[blockIdx], SPACEDIM, iterRng,
                     ops_arg_dat(g_MacroVars[blockIdx], NUMMACROVAR,
                                 LOCALSTENCIL, "double", OPS_READ),
                     ops_arg_reduce(massHandle, 1, "double", OPS_INC));
    }
    ops_reduction_result(massHandle, (double*)totalMass);
}

void NormaliseF(Real* ratio) {
    for (int blockIdx = 0; blockIdx < BlockNum(); blockIdx++) {
        int* iterRng = BlockIterRng(blockIdx, IterRngWhole());
        ops_par_loop(
            KerNormaliseF, "KerNormaliseF", g_Block[blockIdx], SPACEDIM,
            iterRng, ops_arg_gbl(ratio, 1, "double", OPS_READ),
            ops_arg_dat(g_f[blockIdx], NUMXI, LOCALSTENCIL, "double", OPS_RW));
    }
}
void CalcResidualError() {
    for (int macroVarIdx = 0; macroVarIdx < MacroVarsNum(); macroVarIdx++) {
        for (int blockIdx = 0; blockIdx < BlockNum(); blockIdx++) {
            int* iterRng = BlockIterRng(blockIdx, IterRngWhole());
            ops_par_loop(KerCalcMacroVarSquareofDifference,
                         "KerCalcMacroVarSquareofDifference", g_Block[blockIdx],
                         SPACEDIM, iterRng,
                         ops_arg_dat(g_MacroVars[blockIdx], NUMMACROVAR,
                                     LOCALSTENCIL, "double", OPS_READ),
                         ops_arg_dat(g_MacroVarsCopy[blockIdx], NUMMACROVAR,
                                     LOCALSTENCIL, "double", OPS_READ),
                         ops_arg_gbl(&macroVarIdx, 1, "int", OPS_READ),
                         ops_arg_reduce(g_ResidualErrorHandle[macroVarIdx], 1,
                                        "double", OPS_INC));
        }
    }
    for (int macroVarIdx = 0; macroVarIdx < MacroVarsNum(); macroVarIdx++) {
        ops_reduction_result(g_ResidualErrorHandle[macroVarIdx],
                             (double*)&g_ResidualError[2 * macroVarIdx]);
    }
    for (int blockIdx = 0; blockIdx < BlockNum(); blockIdx++) {
        int* iterRng = BlockIterRng(blockIdx, IterRngWhole());
        ops_par_loop(KerCopyMacroVars, "KerCopyMacroVars", g_Block[blockIdx],
                     SPACEDIM, iterRng,
                     ops_arg_dat(g_MacroVars[blockIdx], NUMMACROVAR,
                                 LOCALSTENCIL, "double", OPS_READ),
                     ops_arg_dat(g_MacroVarsCopy[blockIdx], NUMMACROVAR,
                                 LOCALSTENCIL, "double", OPS_RW));
    }
    for (int macroVarIdx = 0; macroVarIdx < MacroVarsNum(); macroVarIdx++) {
        for (int blockIdx = 0; blockIdx < BlockNum(); blockIdx++) {
            int* iterRng = BlockIterRng(blockIdx, IterRngWhole());
            ops_par_loop(KerCalcMacroVarSquare, "KerCalcMacroVarSquare",
                         g_Block[blockIdx], SPACEDIM, iterRng,
                         ops_arg_dat(g_MacroVars[blockIdx], NUMMACROVAR,
                                     LOCALSTENCIL, "double", OPS_READ),
                         ops_arg_gbl(&macroVarIdx, 1, "int", OPS_READ),
                         ops_arg_reduce(g_ResidualErrorHandle[macroVarIdx], 1,
                                        "double", OPS_INC));
        }
    }
    for (int macroVarIdx = 0; macroVarIdx < MacroVarsNum(); macroVarIdx++) {
        ops_reduction_result(g_ResidualErrorHandle[macroVarIdx],
                             (double*)&g_ResidualError[2 * macroVarIdx + 1]);
    }
}

void ForwardEuler() {
    for (int blockIndex = 0; blockIndex < BlockNum(); blockIndex++) {
        int* iterRng = BlockIterRng(blockIndex, IterRngWhole());
        ops_par_loop(KerCutCellCVTUpwind2nd, "KerCutCellCVTUpwind2nd",
                     g_Block[blockIndex], SPACEDIM, iterRng,
                     ops_arg_dat(g_CoordinateXYZ[blockIndex], SPACEDIM,
                                 ONEPTREGULARSTENCIL, "double", OPS_READ),
                     ops_arg_dat(g_NodeType[blockIndex], NUMCOMPONENTS,
                                 LOCALSTENCIL, "int", OPS_READ),
                     ops_arg_dat(g_GeometryProperty[blockIndex], 1,
                                 LOCALSTENCIL, "int", OPS_READ),
                     ops_arg_dat(g_f[blockIndex], NUMXI, ONEPTREGULARSTENCIL,
                                 "double", OPS_READ),
                     ops_arg_dat(g_fStage[blockIndex], NUMXI, LOCALSTENCIL,
                                 "double", OPS_RW));
        Real schemeCoeff{1};
        ops_par_loop(KerCutCellExplicitTimeMach, "KerCutCellExplicitTimeMach",
                     g_Block[blockIndex], SPACEDIM, iterRng,
                     ops_arg_gbl(pTimeStep(), 1, "double", OPS_READ),
                     ops_arg_gbl(&schemeCoeff, 1, "double", OPS_READ),
                     ops_arg_dat(g_NodeType[blockIndex], NUMCOMPONENTS,
                                 LOCALSTENCIL, "int", OPS_READ),
                     ops_arg_dat(g_GeometryProperty[blockIndex], 1,
                                 LOCALSTENCIL, "int", OPS_READ),
                     ops_arg_dat(g_fStage[blockIndex], NUMXI, LOCALSTENCIL,
                                 "double", OPS_READ),
                     ops_arg_dat(g_feq[blockIndex], NUMXI, LOCALSTENCIL,
                                 "double", OPS_READ),
                     ops_arg_dat(g_Tau[blockIndex], NUMCOMPONENTS, LOCALSTENCIL,
                                 "double", OPS_READ),
                     ops_arg_dat(g_Bodyforce[blockIndex], NUMXI, LOCALSTENCIL,
                                 "double", OPS_READ),
                     ops_arg_dat(g_f[blockIndex], NUMXI, LOCALSTENCIL, "double",
                                 OPS_RW));
    }
}

void DispResidualError(const int iter, const Real checkPeriod) {
    ops_printf("##########Residual Error at %i time step##########\n", iter);
    for (int macroVarIdx = 0; macroVarIdx < MacroVarsNum(); macroVarIdx++) {
        Real residualError = g_ResidualError[2 * macroVarIdx] /
                             g_ResidualError[2 * macroVarIdx + 1] /
                             (checkPeriod * TimeStep());
        ops_printf("%s = %.17g\n", MacroVarName()[macroVarIdx].c_str(),
                   residualError);
    }
}

// void StreamCollisionSWE() {
//     UpdateMacroVars();
//     // Real TotalMass{0};
//     // CalcTotalMass(&TotalMass);
//     // Real Ratio{TotalMass/TotalMeshSize()};
//     // NormaliseF(&Ratio);
//     // UpdateMacroVars();
//     CopyDistribution(g_f, g_fStage);
//     UpdateSWEFeqandBodyforce();
//     UpdateSWETau();
//     Collision();
//     Stream();
//     ImplementBoundary();
// }

void StreamCollision() {
    UpdateMacroVars();
    //ops_printf("Varibale have beeen updated in stream collision. \n");

    // Real TotalMass{0};
    // CalcTotalMass(&TotalMass);
    // Real Ratio{TotalMass/TotalMeshSize()};
    // NormaliseF(&Ratio);
    // UpdateMacroVars();

    // ops_printf("\n Updated macro vars ");

    CopyDistribution(g_f, g_fStage);
    //ops_printf("Distribution function copied. \n");

    UpdateFeqandBodyforce();
    //ops_printf("Feq and Body Force have been updated. \n");

    UpdateTau();
    //ops_printf("Tau has been updated. \n");

    Collision();
    //ops_printf("Collision done. \n");

    Stream();
    //ops_printf("Streaming done. \n");

    ImplementBoundaryConditions();
}

// void TimeMarching() {
//    UpdateMacroVars();
//    //CopyDistribution(g_f, g_fStage);
//    UpdateSWEFeqandBodyforce();
//    UpdateSWETau();
//    ForwardEuler();
//    //ops_halo_transfer(HaloGroups);
//    ImplementBoundary();
// }

// void TimeMarching() {
//     UpdateMacroVars();
//     //CopyDistribution(g_f, g_fStage);
//     UpdateFeqandBodyforce();
//     UpdateTau();
//     ForwardEuler();
//     //ops_halo_transfer(HaloGroups);
//     ImplementBoundary();
// }

// void StreamCollision(Real utop) {
//     UpdateMacroVars();
//     CopyDistribution(g_f, g_fStage);
//     UpdateFeqandBodyforce();
//     UpdateTau();
//     Collision();
//     Stream();
//     PostCollisionBoundary(utop);
#endif /* OPS_2D */
// }
