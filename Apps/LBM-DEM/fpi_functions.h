/*
 * fpi_functions.h
 *
 *  Created on: Jul 7, 2021
 *      Author: jpd38567
 */

#ifndef FPI_FUNCTIONS_H_
#define FPI_FUNCTIONS_H_

#include "fpi_data.h"
#include <memory>
#include <vector>
#include <map>
#include <string>
#include "type.h"
#include "block_particles.h"
#include "dem_data.h"
#include "particle_mapping.h"
#include "mapping_particles.h"
#include "fsi_models.h"


void DefineInteractionModels(std::vector<FluidParticleModel> particleModels,
						     std::vector<int> compoId, std::vector<Real> variables,
							 SizeType timeStep=0);

void DefineNone(int iComponent, int spaceDim, FluidParticleModel model);
void DefinePsm(int iComponent, int spaceDim, FluidParticleModel model,
				std::vector<Real> variables, SizeType timeStep = 0);
void DefinePrati(int iComponent, int spaceDim, FluidParticleModel model,
				std::vector<Real> variables, SizeType timeStep = 0);

void FSIVelocityFunctions(std::shared_ptr<FpiData>& fpiPtr);
void FsiForceFunction(std::shared_ptr<FpiData>& fpiPtr);
void FsiPreCollisionFunction(std::shared_ptr<FpiData>& fpiPtr);
void FsiCollisionFunction(std::shared_ptr<FpiData>& fpiPtr);
void FsiPostCollisionFunction(std::shared_ptr<FpiData>& fpiPtr);
void FsiCalculateDragForce(std::shared_ptr<FpiData>& fpiPtr);
void ObtainData(std::shared_ptr<FpiData>& fpiPtr, int* idVel, int* loop, Real& tauCompo,
		CollisionType& collisModel, int& idComponent,int& rhoId,int &thId);
void FsiInitializeFunction(std::shared_ptr<FpiData>& fpiPtr);

#endif /* APPS_LBM_DEM_NOP_FPI_FUNCTIONS_H_ */
