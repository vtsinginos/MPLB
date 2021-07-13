#ifndef PARTICLE_MAPPING_KERNEL_INC
#define PARTICLE_MAPPING_KERNEL_INC

#ifdef OPS_MPI
#include "ops_mpi_core.h"
#endif
#include "type.h"
#include "model.h"
#include "ops_seq_v2.h"
#include "flowfield_host_device.h"
#include "model_host_device.h"

void KerInitializePorousGrid(ACC<Real>& sf, ACC<Real>& vP, ACC<Real>& xAvg, ACC<int>& idp,
		const int* sizeReal, const int* sizeInt) {

	int sizeI = (*sizeInt);

	for (int iDir = 0 ; iDir < sizeReal[0]; iDir++)
		sf(iDir, 0, 0, 0) = 0.0;

	for (int iDir = 0; iDir < sizeReal[1]; iDir++)
		vP(iDir, 0, 0, 0) = 0.0;

	for (int iDir = 0; iDir < sizeReal[2]; iDir++)
		xAvg(iDir, 0, 0, 0) = -100.0;

	for (int iDir = 0; iDir < sizeI; iDir++)
		idp(iDir, 0, 0, 0) = -1;
}


#endif
