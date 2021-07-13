/*
 * prati.h
 *
 *  Created on: Jul 7, 2021
 *      Author: jpd38567
 */

#ifndef PRATI_H_
#define PRATI_H_

#include "block_particles.h"
#include "fpi_data.h"
#include "mapping_particles.h"
#include "particle_mapping.h"
#include <memory>
#include <vector>
#include "dem_data.h"
#include "block.h"
#include "flowfield.h"
#include "scheme.h"
#include "type.h"

void FsiInitializePrati(std::shared_ptr<FpiData>& fpiPtr);
void FsiPostVelocitiesPrati(std::shared_ptr<FpiData>& fpiPtr);
void FsiForcePrati(std::shared_ptr<FpiData>& fpiPtr);
void FsiCollisionsPrati(std::shared_ptr<FpiData>& fpiPtr);
void CalculateDragPrati(std::shared_ptr<FpiData>& fpiPtr);

#endif /* APPS_LBM_DEM_NOP_PRATI_H_ */
