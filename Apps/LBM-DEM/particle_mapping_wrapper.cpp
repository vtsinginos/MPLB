/*
 * particle_mapping_wrapper.cpp
 *
 *  Created on: Jul 6, 2021
 *      Author: jpd38567
 */


#include "particle_mapping.h"
#include "grid_mapping.h"
#include "flowfield.h"
#include "scheme.h"
#include "particle_mapping_kernel.inc"
#include <memory.h>
#include "block_particles.h"
#include "grid_mapping.h"
void InitializePorousLists(std::shared_ptr<MappingParticles>& mappingPtr, int component) {

	int sizeReal[3], sizeInt[1];
	int spaceDim = mappingPtr->GetSpaceDim();
	for (int iType = 0; iType < 3; iType++)
		sizeReal[iType] = mappingPtr->SizeAtRealType(iType);

	sizeInt[0] = mappingPtr->SizeAtIntType(0);

	for (const auto& idBlock : BlockParticleList) {
		BlockParticles ParticleCurrentBlock = idBlock.second;
		std::vector<int> iterRng;
		iterRng.assign(ParticleCurrentBlock.GetBlock().WholeRange().begin(),
					   ParticleCurrentBlock.GetBlock().WholeRange().end());

		const int blockIndex{ParticleCurrentBlock.GetBlock().ID()};
		ops_par_loop(KerInitializePorousGrid, "KerInitializePorousGrid",
					 ParticleCurrentBlock.GetBlock().Get(), spaceDim, iterRng.data(),
					 ops_arg_dat(mappingPtr->GetRealField(0, blockIndex), sizeReal[0],
							 	 LOCALSTENCIL, "double", OPS_WRITE),
					 ops_arg_dat(mappingPtr->GetRealField(1, blockIndex), sizeReal[1],
							 	 LOCALSTENCIL, "double", OPS_WRITE),
				     ops_arg_dat(mappingPtr->GetRealField(2, blockIndex), sizeReal[2],
				    		 	 LOCALSTENCIL, "double", OPS_WRITE),
					 ops_arg_dat(mappingPtr->GetIntField(0, blockIndex), sizeInt[0],
							 	 LOCALSTENCIL, "int", OPS_WRITE),
					 ops_arg_gbl(sizeReal, 3, "int", OPS_READ),
					 ops_arg_gbl(sizeInt, 1, "int", OPS_READ));
	}



}

/*
 * 	for (const auto& idBlock : BlockParticleList) {
		BlockParticles particleCurrentBlock = idBlock.second;
		if (!particleCurrentBlock.owned) continue;
		int blockIndex = particleCurrentBlock.GetBlock().ID();
		nlocal = particleCurrentBlock.NParticles +
				 particleCurrentBlock.NPeriodic;


	}
 */
