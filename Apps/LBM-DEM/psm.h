/*
 * psm.h
 *
 *  Created on: Jul 7, 2021
 *      Author: jpd38567
 */

#ifndef PSM_H_
#define PSM_H_

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

void FsiInitializePSM(std::shared_ptr<FpiData>& fpiPtr);
void FsiForcePSM(std::shared_ptr<FpiData>& fpiPtr);
void FsiCollisionsPSM(std::shared_ptr<FpiData>& fpiPtr);
void CalculateDragPSM(std::shared_ptr<FpiData>& fpiPtr);



#endif /* APPS_LBM_DEM_NOP_PSM_H_ */
