/*
 * force_fsi.h
 *
 *  Created on: Jul 9, 2021
 *      Author: jpd38567
 */

#ifndef FORCE_FSI_H_
#define FORCE_FSI_H_

#include "flowfield.h"
#include "type.h"
#include <vector>
#include "force_fsi_host_device.h"

struct Forces {
	std::vector<Real> forceDefinition;
	ForceType model;
};


void DefineForceModel(std::vector<Real>& params, ForceType forceModel);
Forces& ForceModel();
#endif /* APPS_LBM_DEM_NOP_FORCE_FSI_H_ */
