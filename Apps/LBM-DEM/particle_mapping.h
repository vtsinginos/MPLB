/*
 * particle_mapping.h
 *
 *  Created on: Jul 5, 2021
 *      Author: jpd38567
 */

#ifndef PARTICLE_MAPPING_H_
#define PARTICLE_MAPPING_H_

#include <vector>
#include <map>
#include <string>
#include <memory>
#include "dem_data.h"
#include "mapping_particles.h"
#include "model.h"
#include "flowfield.h"
#include "mapping_models.h"

void DefineParticleMappingModels(std::vector<ParticleMappingModel> particleMappingModels,
		 std::vector<int> compoId, std::vector<int> copyFrom, ParticleShapeDiscriptor particleShape,
		 SizeType timeStep);

void InitializeMappingLists();
void MappingParticlesToLBMGrid();
void UpdateParticlesToLBMGrid();
void WriteParticleMappingToHDF5(SizeType timestep);
void InitializePorousLists(std::shared_ptr<MappingParticles>& mappingPtr,
						   int component);
std::map<int, std::shared_ptr<MappingParticles>>& GetMappingModels();
#endif /* APPS_LBM_DEM_NOP_PARTICLE_MAPPING_H_ */
