/*
 * poros_spherical.h
 *
 *  Created on: Jul 6, 2021
 *      Author: jpd38567
 */

#ifndef POROS_SPHERICAL_H_
#define POROS_SPHERICAL_H_

#include "type.h"
#include "flowfield.h"
#include "scheme.h"
#include "dem_data.h"
#include "mapping_particles.h"
#include <vector>
#include <memory>

void ParticleProjectionSphere(std::shared_ptr<MappingParticles>& mappingPtr,
		  int component);
void UpdateProjectionSphere(std::shared_ptr<MappingParticles>& mappingPtr,
		   int component);





#endif /* APPS_LBM_DEM_NOP_POROS_SPHERICAL_H_ */
