/*
 * spherical mapping.h
 *
 *  Created on: Jul 6, 2021
 *      Author: jpd38567
 */

#ifndef GRID_MAPPING_H_
#define GRID_MAPPING_H_

#include "type.h"
#include "flowfield.h"
#include "scheme.h"
#include "dem_data.h"
#include "mapping_particles.h"
#include <vector>
#include <memory>

void ParticleProjectionSphereGrid(std::shared_ptr<MappingParticles>& mappingPtr,
								  int component);
void UpdateProjectectionSphereGrid(std::shared_ptr<MappingParticles>& mappingPtr,
								   int component);

#endif /* APPS_LBM_DEM_NOP_SPHERICAL_MAPPING_H_ */
