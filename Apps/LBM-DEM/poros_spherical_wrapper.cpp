/*
 * poros_spherical.cpp
 *
 *  Created on: Jul 6, 2021
 *      Author: jpd38567
 */

#include "flowfield.h"
#include "flowfield_host_device.h"
#include "scheme.h"

#include "poros_spherical.h"
#include "block_particles.h"

#include "ops_seq_v2.h"
#include "poros_spherical_kernel.inc"


void ParticleProjectionSphere(std::shared_ptr<MappingParticles>& mappingPtr,
		  	  	  	  	      int component) {

	int sizeReal[3], sizeInt, idParticle, blockIndex, nPoints, nelem;
	int spaceDim = mappingPtr->GetSpaceDim();
	int stenList[2 * spaceDim];
	Real Dx, xPos[spaceDim], Radius, uPart[spaceDim], omPart[spaceDim];
	for (int iDim = 0; iDim < 3; iDim++)
		sizeReal[iDim] = mappingPtr->SizeAtRealType(iDim);
	nPoints = 9;
	sizeInt = mappingPtr->SizeAtIntType(0);
	nelem = mappingPtr->NumberOfElements();
	Dx = GetDx();
	for (const auto& idBlock : BlockParticleList) {
		BlockParticles particleCurrentBlock = idBlock.second;
		if (!particleCurrentBlock.owned) continue;
		blockIndex = particleCurrentBlock.GetBlock().ID();
		int nlocal =  particleCurrentBlock.NParticles +
					  particleCurrentBlock.NPeriodic;

		for (int iPart = 0; iPart < nlocal; iPart++) {

			for (int iDim = 0; iDim < spaceDim; iDim++) {
				xPos[iDim] = particleCurrentBlock.particleList.at(iPart).xParticle[iDim];
				uPart[iDim] = particleCurrentBlock.particleList.at(iPart).uParticle[iDim];
				omPart[iDim] = particleCurrentBlock.particleList.at(iPart).omegaParticle[iDim];
			}
			idParticle = iPart;
			Radius = particleCurrentBlock.particleList.at(iPart).particleShape->Rparticle;
			for (int iDir = 0; iDir < 2 * spaceDim; iDir++)
				stenList[iDir] = particleCurrentBlock.particleList.at(iPart).stenList[iDir];
			//printf("StenList[%d] = [%d %d %d %d %d %d]\n", iPart, stenList[0], stenList[1], stenList[2], stenList[3], stenList[4], stenList[5]);
			ops_par_loop(KerSolidFracSphere, "KerSolidFracSphere",
						 particleCurrentBlock.GetBlock().Get(), spaceDim,  stenList,
						 ops_arg_dat(mappingPtr->GetIntField(0, blockIndex), sizeInt,
								 	 LOCALSTENCIL,"int", OPS_RW),
						 ops_arg_dat(mappingPtr->GetRealField(0, blockIndex), sizeReal[0],
									 LOCALSTENCIL, "double", OPS_RW),
						 ops_arg_dat(mappingPtr->GetRealField(1, blockIndex), sizeReal[1],
									 LOCALSTENCIL, "double", OPS_RW),
						 ops_arg_dat(mappingPtr->GetRealField(2, blockIndex), sizeReal[2],
								     LOCALSTENCIL, "double", OPS_RW),
						 ops_arg_dat(g_CoordinateXYZ()[blockIndex], spaceDim,
								 	 LOCALSTENCIL, "double", OPS_READ),
					     ops_arg_gbl(xPos, spaceDim, "double", OPS_READ),
					     ops_arg_gbl(&Radius, 1, "double", OPS_READ),
					     ops_arg_gbl(&idParticle, 1, "double", OPS_READ),
						 ops_arg_gbl(uPart, spaceDim, "double", OPS_READ),
						 ops_arg_gbl(omPart, spaceDim, "double", OPS_READ),
						 ops_arg_gbl(&Dx, 1, "double", OPS_READ),
						 ops_arg_gbl(&nelem, 1, "double", OPS_READ),
						 ops_arg_gbl(&spaceDim, 1, "double", OPS_READ));
		}
	}

}

void UpdateProjectionSphere(std::shared_ptr<MappingParticles>& mappingPtr,
	  	      	  	  	    int component) {


	int sizeReal[3], sizeInt, idParticle, blockIndex, nelem;
	int spaceDim = mappingPtr->GetSpaceDim();
	int stenList[2 * spaceDim];
	Real xPar[spaceDim], radius, velP[spaceDim], omP[spaceDim];
	Real dx = GetDx();

	for (int iDim = 0; iDim < 3; iDim++)
		sizeReal[iDim] = mappingPtr->SizeAtRealType(iDim);
	sizeInt = mappingPtr->SizeAtIntType(0);
	nelem = mappingPtr->NumberOfElements();

	for (auto& idBlock : BlockParticleList) {
		BlockParticles particleCurrentBlock = idBlock.second;
		if ( !particleCurrentBlock.owned ) continue;
		int blockIndex = particleCurrentBlock.GetBlock().ID();
		int nlocal =  particleCurrentBlock.NParticles +
				particleCurrentBlock.NPeriodic;

		for (int iPart = 0; iPart < nlocal; iPart++) {
			for (int iDim = 0; iDim < spaceDim; iDim++) {
				xPar[iDim] = particleCurrentBlock.particleList.at(iPart).xParticle[iDim];
				velP[iDim] = particleCurrentBlock.particleList.at(iPart).uParticle[iDim];
				omP[iDim] = particleCurrentBlock.particleList.at(iPart).omegaParticle[iDim];
			}
			radius = particleCurrentBlock.particleList.at(iPart).particleShape->Rparticle;
			idParticle = iPart;
			for (int iDir = 0; iDir < 2 * spaceDim; iDir++)
				stenList[iDir] = particleCurrentBlock.particleList.at(iPart).stenList[iDir];

			ops_par_loop(KerPorousSphericalUpdate, "KerPorousSphericalUpdate",
						 particleCurrentBlock.GetBlock().Get(), spaceDim, stenList,
						 ops_arg_dat(mappingPtr->GetRealField(1, blockIndex), sizeReal[1],
								 	 LOCALSTENCIL, "Real" , OPS_READ),
						 ops_arg_dat(mappingPtr->GetIntField(0, blockIndex), sizeInt,
								 	 LOCALSTENCIL, "int", OPS_READ),
						 ops_arg_dat(mappingPtr->GetRealField(2, blockIndex), sizeReal[2],
								 	 LOCALSTENCIL, "double", OPS_READ),
						 ops_arg_gbl(&idParticle, 1, "int", OPS_READ),
						 ops_arg_gbl(xPar, spaceDim, "double", OPS_READ),
					     ops_arg_gbl(&radius, 1, "double", OPS_READ),
						 ops_arg_gbl(velP, spaceDim, "double", OPS_READ),
					     ops_arg_gbl(omP, spaceDim, "double", OPS_READ),
						 ops_arg_gbl(&dx, 1, "double", OPS_READ),
						 ops_arg_gbl(&spaceDim, 1, "int", OPS_READ),
						 ops_arg_gbl(&nelem, 1, "int", OPS_READ));
		}
	}
}