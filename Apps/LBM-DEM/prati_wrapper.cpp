/*
 * prati_wrapper.cpp
 *
 *  Created on: Jul 12, 2021
 *      Author: jpd38567
 */

#include "prati.h"

#include "flowfield_host_device.h"
#include "ops_seq_v2.h"
#include "force_fsi.h"

#include "prati_kernel.inc"

void FsiInitializePrati(std::shared_ptr<FpiData>& fpiPtr) {

	int spaceDim = fpiPtr->GetSpaceDim();
	int size = fpiPtr->SizeAtRealType(0);

	for (const auto& idBlock : g_Block()) {
		const Block& block{idBlock.second};
		std::vector<int> iterRng;
		iterRng.assign(block.WholeRange().begin(), block.WholeRange().end());
		const int blockIndex{block.ID()};
		ops_par_loop(KerInitializePrati,"KerInitializePSM", block.Get(),
					 spaceDim, iterRng.data(),
					 ops_arg_dat(fpiPtr->GetRealField(0, blockIndex), size,
							 	 LOCALSTENCIL,"double", OPS_WRITE),
					 ops_arg_gbl(&size, 1, "int", OPS_READ));

	}

}

void FsiCollisionsPrati(std::shared_ptr<FpiData>& fpiPtr) {

	int spaceDim, noElem, componentID,size;
	int sizeR1, sizeR2, sizeI1;
	spaceDim = fpiPtr->GetSpaceDim();

	Real tauRef;
	Component compo{fpiPtr->FluidComponent()}; //TO IF ICANT I need to

	Real gamma = fpiPtr->GetRealVariableAt(0);


	Forces forceParams{ForceModel()};
	int sizeForce = forceParams.forceDefinition.size();
	Real force[sizeForce];

	for (int iDir = 0; iDir < sizeForce; iDir++)
		force[iDir] = forceParams.forceDefinition[iDir];

	int forceFlag = (int) forceParams.model;
	tauRef = compo.tauRef;
	componentID = compo.id;
	const Real* pdt{pTimeStep()};
	noElem = fpiPtr->GetNElem();
	size = fpiPtr->SizeAtRealType(0);
	std::shared_ptr<MappingParticles> mappingModel = GetMappingModels().at(componentID);
	sizeR1 = mappingModel->SizeAtRealType(0);
	sizeR2 = mappingModel->SizeAtRealType(1);
	sizeI1 = mappingModel->SizeAtIntType(0);

	for (const auto& idBlock : g_Block()) {
		const Block& block{idBlock.second};
		std::vector<int> iterRng;
		iterRng.assign(block.WholeRange().begin(), block.WholeRange().end());
		const int blockIndex{block.ID()};
		ops_par_loop(KerCollisionPrati, "KerCollisionPrati", block.Get(),
					 spaceDim, iterRng.data(),
					 ops_arg_dat(g_fStage()[blockIndex], NUMXI, LOCALSTENCIL,
							 	 "double", OPS_WRITE),
					 ops_arg_dat(fpiPtr->GetRealField(0, blockIndex), size, LOCALSTENCIL,
							 	 "double", OPS_WRITE),
					 ops_arg_dat(g_f()[blockIndex], NUMXI, LOCALSTENCIL,
							 	 "double", OPS_READ),
					 ops_arg_dat(g_CoordinateXYZ()[blockIndex], spaceDim,
								 LOCALSTENCIL, "double", OPS_READ),
					 ops_arg_dat(g_NodeType().at(compo.id).at(blockIndex), 1,
								 LOCALSTENCIL, "double", OPS_READ),
					 ops_arg_dat(g_MacroVars().at(compo.macroVars.at(Variable_Rho).id)
								 .at(blockIndex), 1, LOCALSTENCIL, "double", OPS_READ),
					 ops_arg_dat(g_MacroVars().at(compo.uId).at(blockIndex),
								 1, LOCALSTENCIL, "double", OPS_READ),
					 ops_arg_dat(g_MacroVars().at(compo.vId).at(blockIndex),
								 1, LOCALSTENCIL, "double", OPS_READ),
					 ops_arg_dat(g_MacroVars().at(compo.wId).at(blockIndex),
								 1, LOCALSTENCIL, "double", OPS_READ),
					 ops_arg_dat(mappingModel->GetIntField(0, blockIndex), sizeI1,
							 	 LOCALSTENCIL, "int", OPS_READ),
					 ops_arg_dat(mappingModel->GetRealField(0, blockIndex), sizeR1,
							 	 LOCALSTENCIL, "double", OPS_READ),
					 ops_arg_dat(mappingModel->GetRealField(1, blockIndex), sizeR2,
							 	 LOCALSTENCIL, "double", OPS_READ),
					 ops_arg_gbl(pdt, 1, "double", OPS_READ),
					 ops_arg_gbl(&tauRef, 1, "int", OPS_READ),
					 ops_arg_gbl(&forceFlag, 1, "int", OPS_READ),
					 ops_arg_gbl(force, sizeForce, "double", OPS_READ),
					 ops_arg_gbl(&sizeForce, 1, "int", OPS_READ),
					 ops_arg_gbl(&gamma, 1, "double", OPS_READ),
					 ops_arg_gbl(&noElem, 1, "int", OPS_READ),
					 ops_arg_gbl(&spaceDim, 1, "int", OPS_READ),
					 ops_arg_gbl(compo.index, 2, "int", OPS_READ));
	}


}

void FsiPostVelocitiesPrati(std::shared_ptr<FpiData>& fpiPtr) {

	int spaceDim, noElem, componentID,size;
	int sizeR1, sizeR2, sizeI1;
	spaceDim = fpiPtr->GetSpaceDim();

	Real tauRef;
	Component compo{fpiPtr->FluidComponent()}; //TO IF ICANT I need to

	Real gamma = fpiPtr->GetRealVariableAt(0);


	Forces forceParams{ForceModel()};
	int sizeForce = forceParams.forceDefinition.size();
	Real force[sizeForce];

	for (int iDir = 0; iDir < sizeForce; iDir++)
		force[iDir] = forceParams.forceDefinition[iDir];

	int forceFlag = (int) forceParams.model;
	tauRef = compo.tauRef;
	componentID = compo.id;
	const Real* pdt{pTimeStep()};
	noElem = fpiPtr->GetNElem();
	size = fpiPtr->SizeAtRealType(0);
	std::shared_ptr<MappingParticles> mappingModel = GetMappingModels().at(componentID);
	sizeR1 = mappingModel->SizeAtRealType(0);
	sizeR2 = mappingModel->SizeAtRealType(1);
	sizeI1 = mappingModel->SizeAtIntType(0);

	for (const auto& idBlock : g_Block()) {
		const Block& block(idBlock.second);
		 std::vector<int> iterRng;
		 iterRng.assign(block.WholeRange().begin(), block.WholeRange().end());
		 const int blockIndex{block.ID()};
		 for (auto& macroVar : compo.macroVars) {
			 const int varId{macroVar.second.id};
			 const VariableTypes varType{macroVar.first};
			 switch (varType) {
			 	case Variable_U:
			 		 ops_par_loop(KerPratiUpdateU3D, "KerPratiUpdateU3D", block.Get(),
			 					  spaceDim, iterRng.data(),
			 					  ops_arg_dat(g_MacroVars().at(varId).at(blockIndex),
			 							  	  1, LOCALSTENCIL, "double", OPS_RW),
								  ops_arg_dat(g_NodeType().at(compo.id).at(blockIndex), 1,
			 								  LOCALSTENCIL, "int", OPS_READ),
								  ops_arg_dat(mappingModel->GetIntField(0, blockIndex), sizeI1,
										  	  LOCALSTENCIL, "int", OPS_READ),
								  ops_arg_dat(mappingModel->GetRealField(0, blockIndex), sizeR1,
										  	  LOCALSTENCIL, "double", OPS_READ),
								  ops_arg_dat(mappingModel->GetRealField(1, blockIndex), sizeR2,
											  LOCALSTENCIL, "double", OPS_READ),
								  ops_arg_dat(g_CoordinateXYZ()[blockIndex], spaceDim,
											  LOCALSTENCIL, "double", OPS_READ),
			 					  ops_arg_gbl(pdt, 1, "double", OPS_READ),
			 					  ops_arg_gbl(&tauRef, 1, "double", OPS_READ),
			 					  ops_arg_gbl(&gamma, 1, "double", OPS_READ),
			 					  ops_arg_gbl(&forceFlag, 1, "int", OPS_READ),
			 					  ops_arg_gbl(force, sizeForce, "double", OPS_READ),
								  ops_arg_gbl(&sizeForce, 1, "int", OPS_READ),
			 					 ops_arg_gbl(&noElem, 1, "int", OPS_READ),
			 					  ops_arg_gbl(&spaceDim, 1, "int", OPS_READ));
			 		 break;
			 	case Variable_V:
			 		ops_par_loop(KerPratiUpdateV3D, "KerPratiUpdateV3D", block.Get(),
			 					 spaceDim, iterRng.data(),
								 ops_arg_dat(g_MacroVars().at(varId).at(blockIndex),
										 	 1, LOCALSTENCIL, "double", OPS_RW),
								 ops_arg_dat(g_NodeType().at(compo.id).at(blockIndex),
										 	 1, LOCALSTENCIL, "int", OPS_READ),
								 ops_arg_dat(mappingModel->GetIntField(0, blockIndex), sizeI1,
											 LOCALSTENCIL, "int", OPS_READ),
								 ops_arg_dat(mappingModel->GetRealField(0, blockIndex), sizeR1,
											  LOCALSTENCIL, "double", OPS_READ),
								 ops_arg_dat(mappingModel->GetRealField(1, blockIndex), sizeR2,
											  LOCALSTENCIL, "double", OPS_READ),
								 ops_arg_dat(g_CoordinateXYZ()[blockIndex], spaceDim,
											 LOCALSTENCIL, "double", OPS_READ),
								 ops_arg_gbl(pdt, 1, "double", OPS_READ),
			 					 ops_arg_gbl(&tauRef, 1, "double", OPS_READ),
			 					 ops_arg_gbl(&gamma, 1, "double", OPS_READ),
			 					 ops_arg_gbl(&forceFlag, 1, "int", OPS_READ),
								 ops_arg_gbl(force, sizeForce, "double", OPS_READ),
								 ops_arg_gbl(&sizeForce, 1, "int", OPS_READ),
			 					 ops_arg_gbl(&noElem, 1, "int", OPS_READ),
			 					 ops_arg_gbl(&spaceDim, 1, "int", OPS_READ));
			 	break;
			 case Variable_W:
			 	ops_par_loop(KerPratiUpdateW3D, "KerPratiUpdateW3D", block.Get(),
			 				 spaceDim, iterRng.data(),
			 				 ops_arg_dat(g_MacroVars().at(varId).at(blockIndex),
			 					 		 1, LOCALSTENCIL, "double", OPS_RW),
			 				 ops_arg_dat(g_NodeType().at(compo.id).at(blockIndex), 1,
			 						 	 LOCALSTENCIL, "int", OPS_READ),
							 ops_arg_dat(mappingModel->GetIntField(0, blockIndex), sizeI1,
										 LOCALSTENCIL, "int", OPS_READ),
							 ops_arg_dat(mappingModel->GetRealField(0, blockIndex), sizeR1,
										 LOCALSTENCIL, "double", OPS_READ),
							 ops_arg_dat(mappingModel->GetRealField(1, blockIndex), sizeR2,
									 	 LOCALSTENCIL, "double", OPS_READ),
							 ops_arg_dat(g_CoordinateXYZ()[blockIndex], spaceDim,
										 LOCALSTENCIL, "double", OPS_READ),
			 				 ops_arg_gbl(pdt, 1, "double", OPS_READ),
			 				 ops_arg_gbl(&tauRef, 1, "double", OPS_READ),
			 				 ops_arg_gbl(&gamma, 1, "double", OPS_READ),
			 				 ops_arg_gbl(&forceFlag, 1, "int", OPS_READ),
							 ops_arg_gbl(force, sizeForce, "double", OPS_READ),
							 ops_arg_gbl(&sizeForce, 1, "int", OPS_READ),
			 				 ops_arg_gbl(&noElem, 1, "int", OPS_READ),
			 			     ops_arg_gbl(&spaceDim, 1, "int", OPS_READ));
			 	break;
			default:
			  break;
			 }
		 }
	}

}




void FsiForcePrati(std::shared_ptr<FpiData>& fpiPtr) {


}

void CalculateDragPrati(std::shared_ptr<FpiData>& fpiPtr) {

	int spaceDim, size, noElem, componentID;
	int blockIndex, idParticle;
	int sizeI1, sizeR1, sizeR3;
	spaceDim = fpiPtr->GetSpaceDim();
	int stencil[2*spaceDim];
	Real tauRef, dx, xPos[spaceDim], FdLocal[spaceDim], TdLocal[spaceDim];
	Real gamma = fpiPtr->GetRealVariableAt(0);
	const Real* pdt{pTimeStep()};
	noElem = fpiPtr->GetNElem();
	size = fpiPtr->SizeAtRealType(0);
	Component compo{fpiPtr->FluidComponent()};
	tauRef = compo.tauRef;
	componentID = compo.id;
	dx = GetDx();

	//GetPorosModel
	std::shared_ptr<MappingParticles> mappingModel = GetMappingModels().at(componentID);
	sizeR1 = mappingModel->SizeAtRealType(0);
	sizeR3 = mappingModel->SizeAtRealType(2);
	sizeI1 = mappingModel->SizeAtIntType(0);

	for (auto& idBlock : BlockParticleList) {
		BlockParticles& particlesCurrentBlock = idBlock.second;
		if (!particlesCurrentBlock.owned) continue;
		blockIndex = particlesCurrentBlock.GetBlock().ID();
		int nLocal = particlesCurrentBlock.NParticles
				+ particlesCurrentBlock.NPeriodic;
		for (int iPart = 0; iPart < nLocal; iPart++) {
			for (int iDir = 0; iDir < spaceDim; iDir++)  {
				xPos[iDir] = particlesCurrentBlock.particleList[iPart].xParticle[iDir];
				TdLocal[iDir] = 0.0;
				FdLocal[iDir] = 0.0;
			}

			idParticle = iPart;
			for (int iDir = 0; iDir < 2 * spaceDim; iDir++)
					stencil[iDir] = particlesCurrentBlock.particleList[iPart].stenList[iDir];

			ops_par_loop(KerDragPrati, "KerDragPrati", particlesCurrentBlock.GetBlock().Get(),
					 	 spaceDim, stencil,
						 ops_arg_dat(mappingModel->GetIntField(0, blockIndex), sizeI1, //id
								 	 LOCALSTENCIL, "int", OPS_READ),
						 ops_arg_dat(mappingModel->GetRealField(0, blockIndex), sizeR1, //sfp
								 	 LOCALSTENCIL, "double", OPS_READ),
						 ops_arg_dat(mappingModel->GetRealField(2, blockIndex), sizeR3, //xf
						 	 	 	 LOCALSTENCIL, "double", OPS_READ),
						 ops_arg_dat(fpiPtr->GetRealField(0, blockIndex), size, //Fd
								 	 LOCALSTENCIL, "double", OPS_READ),
						 ops_arg_gbl(FdLocal, spaceDim, "double", OPS_READ), //Fdp
						 ops_arg_gbl(TdLocal, spaceDim, "double", OPS_READ), //Tdp
						 ops_arg_gbl(&idParticle, 1, "int", OPS_READ), //itdp
						 ops_arg_gbl(xPos, spaceDim, "double", OPS_READ), //xpos
						 ops_arg_gbl(pdt, 1, "double", OPS_READ), //dt
						 ops_arg_gbl(&tauRef, 1, "double", OPS_READ),
						 ops_arg_gbl(&gamma, 1, "double", OPS_READ),
						 ops_arg_gbl(&spaceDim, 1, "int", OPS_READ),
						 ops_arg_gbl(&noElem, 1, "int", OPS_READ));


			for (int iDir = 0; iDir < spaceDim; iDir++) {
				particlesCurrentBlock.particleList[iPart].FDrag[iDir] += FdLocal[iDir] * dx * dx * dx;
				particlesCurrentBlock.particleList[iPart].TDrag[iDir] += TdLocal[iDir] * dx * dx * dx;
			}
		}
	}

}


