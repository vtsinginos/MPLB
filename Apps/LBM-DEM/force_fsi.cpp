/*
 * force_fsi.cpp
 *
 *  Created on: Jul 12, 2021
 *      Author: jpd38567
 */

#include "force_fsi.h"
#include "force_fsi_host_device.h"
Forces forceModel;

void DefineForceModel(std::vector<Real>& params, ForceType forceScheme) {

	if (params.size()>0) {
		forceModel.model = forceScheme;

		for (auto& parameter : params)  {
			forceModel.forceDefinition.push_back(parameter);
		}
	}
	else  {
		forceModel.model = noForce;

		forceModel.forceDefinition.push_back(0);
	}
}

Forces& ForceModel() { return forceModel;};

