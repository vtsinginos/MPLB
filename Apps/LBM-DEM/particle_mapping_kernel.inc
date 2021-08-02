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

/*!
 * @brief   Kernel for initializing ops lists for particle mapping
 * @author  C. Tsigginos
 *
 */


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